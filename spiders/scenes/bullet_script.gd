extends Area2D

@export var speed := 50.0

var direction := Vector2.ZERO


func setup(target_pos: Vector2):
	direction = (target_pos - global_position).normalized()
	rotation = direction.angle()


func _physics_process(delta):
	global_position += direction * speed * delta



	
