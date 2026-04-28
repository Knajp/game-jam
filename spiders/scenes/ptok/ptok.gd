extends AnimatableBody2D

@export var speed := 30.0
@export var wait_time := 3.0
@export var chase_time := 4.0
@export var vulnerable_time := 2.0

var player_url = null
var player_mrc = null
var target = null
var can_be_hit := false

@onready var hitbox_shape = $ptasznik/Area2D/HitboxShape
@onready var collision_shape = $CollisionShape
@onready var sprite = $ptasznik

enum State { IDLE, PREPARE, LEAP, CHASE, LANDING }
var state := State.IDLE


func _ready():
	var spider_duo = get_parent().get_node_or_null("SpiderDuo")
	if spider_duo:
		player_url = spider_duo.get_node_or_null("Ursula")
		player_mrc = spider_duo.get_node_or_null("Martin")

	sprite.sprite_frames.set_animation_speed("default", 5)
	sprite.sprite_frames.set_animation_speed("danger", 5)

	sprite.play("default")
	sprite.pause()
	_enter_idle()


func _physics_process(delta):
	if state == State.CHASE:
		_chase_tick(delta)


func _chase_tick(delta):
	target = get_closest_player()
	if not target:
		return
	var dir = (target.global_position - global_position).normalized()
	move_and_collide(dir * speed * delta)


func _enter_idle():
	state = State.IDLE
	can_be_hit = true
	_restore_collision()

	sprite.play("default")
	sprite.set_frame_and_progress(0, 0.0)
	sprite.pause()

	await get_tree().create_timer(wait_time).timeout
	if state == State.IDLE:
		_enter_prepare()


func _enter_prepare():
	state = State.PREPARE
	can_be_hit = false
	_disable_collision()

	sprite.play("default")

	await get_tree().create_timer(wait_time * 0.5).timeout
	_enter_leap()


func _enter_leap():
	sprite.position = Vector2(0, 0)
	state = State.LEAP
	var start_y = position.y

	# wylot w górę
	var tween = create_tween()
	tween.tween_property(self, "position:y", start_y - 120, 0.4)\
		.set_trans(Tween.TRANS_SINE)\
		.set_ease(Tween.EASE_OUT)

	await tween.finished

	# WYKRZYKNIK + start spadania (klatki 4–8)
	sprite.play("danger")
	sprite.pause()
	sprite.set_frame(4)

	var tween2 = create_tween()

	# ruch w dół
	tween2.parallel().tween_property(self, "position:y", start_y, 0.35)\
		.set_trans(Tween.TRANS_SINE)\
		.set_ease(Tween.EASE_IN)

	# animacja klatek 4 → 8
	tween2.parallel().tween_method(
		func(v): sprite.set_frame(int(v)),
		4, 8, 0.35
	)

	await tween2.finished
	sprite.set_frame(8)

	_enter_chase()


func _enter_chase():
	state = State.CHASE
	target = get_closest_player()

	sprite.play("danger")
	
	await get_tree().create_timer(chase_time).timeout
	if state == State.CHASE:
		_enter_landing()


func _enter_landing():
	sprite.position = Vector2(24.0, -100.0)
	state = State.LANDING
	can_be_hit = true
	_restore_collision()

	sprite.play("default")

	for i in range(4, 9):
		sprite.set_frame(i)
		await get_tree().create_timer(0.5).timeout

	# powrót do stanu spoczynkowego
	sprite.set_frame_and_progress(0, 0.0)
	sprite.pause()

	earthquake()

	await get_tree().create_timer(vulnerable_time).timeout
	_enter_idle()


func _disable_collision():
	hitbox_shape.set_deferred("disabled", true)
	collision_shape.set_deferred("disabled", true)


func _restore_collision():
	hitbox_shape.set_deferred("disabled", false)
	collision_shape.set_deferred("disabled", false)


func get_closest_player():
	if not player_url or not player_mrc:
		return null

	var d1 = global_position.distance_to(player_url.global_position)
	var d2 = global_position.distance_to(player_mrc.global_position)

	return player_url if d1 < d2 else player_mrc


func earthquake():
	pass
