extends CharacterBody2D

@export var speed := 5.0
@export var max_distance := 100.0
@export var fire_rate := 2.5

var player_url = null
var player_mrc = null
var target = null
var can_shoot := true

@onready var ray = $RayCast2D
@onready var bullet_scene = preload("res://scenes/bullet.tscn")


func _ready():
	var spider_duo = get_tree().current_scene.get_node_or_null("SpiderDuo")

	print("[ENEMY] SpiderDuo node:", spider_duo)

	if spider_duo:
		player_url = spider_duo.get_node_or_null("Ursula")
		player_mrc = spider_duo.get_node_or_null("Martin")

	ray.enabled = true

	await get_tree().process_frame
	target = get_closest_player()

	shoot()


func _physics_process(delta):
	if not player_url or not player_mrc:
		return

	target = get_closest_player()
	if target == null:
		return

	var to_target = target.global_position - global_position
	var distance = to_target.length()

	if distance > max_distance:
		velocity = Vector2.ZERO
		move_and_slide()
		return

	var dir = to_target.normalized()
	velocity = dir * speed
	move_and_slide()

	# =========================
	# SHOOTING
	# =========================
	ray.target_position = to_target
	ray.force_raycast_update()

	var blocked = ray.is_colliding() and ray.get_collider() != target

	if not blocked and can_shoot:
		shoot()

	# =========================
	# ROPE CHECK (NO C++)
	# =========================
	var spider_duo = get_tree().current_scene.get_node_or_null("SpiderDuo")

	if spider_duo:
		if rope_hit_check():
			print("[ENEMY] Hit rope → queue_free")
			queue_free()
			return


# =========================
# ROPE APPROXIMATION (SAFE)
# =========================
func rope_hit_check() -> bool:
	var spider_duo = get_tree().current_scene.get_node_or_null("SpiderDuo")
	if spider_duo == null:
		return false

	var a = spider_duo.get_node_or_null("Martin")
	var b = spider_duo.get_node_or_null("Ursula")

	if a == null or b == null:
		return false

	var segments := 12
	var radius := 10.0

	for i in range(segments):
		var t1 = float(i) / segments
		var t2 = float(i + 1) / segments

		var p1 = a.global_position.lerp(b.global_position, t1)
		var p2 = a.global_position.lerp(b.global_position, t2)

		if point_segment_distance(global_position, p1, p2) <= radius:
			return true

	return false


func point_segment_distance(p: Vector2, a: Vector2, b: Vector2) -> float:
	var ab = b - a
	var ap = p - a

	var ab_len2 = ab.length_squared()
	if ab_len2 == 0:
		return p.distance_to(a)

	var t = ap.dot(ab) / ab_len2
	t = clamp(t, 0.0, 1.0)

	var closest = a + ab * t
	return p.distance_to(closest)


# =========================
# TARGETING
# =========================
func get_closest_player():
	if player_url == null or player_mrc == null:
		return null

	var d1 = global_position.distance_to(player_url.global_position)
	var d2 = global_position.distance_to(player_mrc.global_position)

	return player_url if d1 < d2 else player_mrc


# =========================
# SHOOT
# =========================
func shoot():
	if target == null:
		return

	can_shoot = false

	var bullet = bullet_scene.instantiate()
	get_tree().current_scene.add_child(bullet)

	bullet.global_position = global_position
	bullet.setup(target.global_position)

	await get_tree().create_timer(fire_rate).timeout
	can_shoot = true
