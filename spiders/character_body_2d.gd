extends CharacterBody2D

@export var speed := 10.0
@export var max_distance := 100.0
@export var fire_rate := 1.0

var player_url = null
var player_mrc = null
var target = null
var can_shoot := true

@onready var ray = $RayCast2D
@onready var bullet_scene = preload("res://scenes/bullet.tscn")


func _ready():
	var spider_duo = get_parent().get_parent().get_node_or_null("SpiderDuo")
	if spider_duo:
		player_url = spider_duo.get_node_or_null("Ursula")
		player_mrc = spider_duo.get_node_or_null("Martin")
	print(spider_duo)
	print(spider_duo.get_class())
	print(spider_duo.get_script())
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

	# --- ONLY CHECK WALL BLOCKING ---
	ray.target_position = to_target
	ray.force_raycast_update()

	var blocked = ray.is_colliding() and ray.get_collider() != target

	if not blocked and can_shoot:
		shoot()


func get_closest_player():
	if player_url == null or player_mrc == null:
		return null

	var d1 = global_position.distance_to(player_url.global_position)
	var d2 = global_position.distance_to(player_mrc.global_position)

	return player_url if d1 < d2 else player_mrc


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
