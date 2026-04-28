extends Area2D

@export var speed := 50.0
var direction := Vector2.ZERO


func setup(target_pos: Vector2):
	direction = (target_pos - global_position).normalized()
	rotation = direction.angle()


func _physics_process(delta):
	global_position += direction * speed * delta

	# Manual distance check instead of unreliable signals
	var spider_duo = get_tree().current_scene.get_node_or_null("SpiderDuo")
	if spider_duo == null:
		return

	var ursula = spider_duo.get_node_or_null("Ursula")
	var martin = spider_duo.get_node_or_null("Martin")

	if ursula != null:
		if global_position.distance_to(ursula.global_position) < 16.0:
			print("HIT PLAYER: Ursula")
			queue_free()
			spider_duo.takeDamage(10)
			return

	if martin != null:
		if global_position.distance_to(martin.global_position) < 16.0:
			print("HIT PLAYER: Martin")
			queue_free()
			spider_duo.takeDamage(10)
			return
