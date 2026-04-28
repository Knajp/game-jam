extends Area2D

@export var speed := 50.0
var direction := Vector2.ZERO
var game_over_triggered := false


func setup(target_pos: Vector2):
	direction = (target_pos - global_position).normalized()
	rotation = direction.angle()
	print("[SETUP] Direction set:", direction)


func _physics_process(delta):
	global_position += direction * speed * delta

	if game_over_triggered:
		return

	var spider_duo = get_tree().current_scene.get_node_or_null("SpiderDuo")
	if spider_duo == null:
		print("[DEBUG] SpiderDuo NOT FOUND")
		return

	var ursula = spider_duo.get_node_or_null("Ursula")
	var martin = spider_duo.get_node_or_null("Martin")

	if ursula and global_position.distance_to(ursula.global_position) < 16.0:
		print("[HIT] Ursula hit → GAME OVER")
		_trigger_game_over()
		return

	if martin and global_position.distance_to(martin.global_position) < 16.0:
		print("[HIT] Martin hit → GAME OVER")
		_trigger_game_over()
		return


func _trigger_game_over():
	if game_over_triggered:
		return

	game_over_triggered = true
	print("[GAME OVER] Triggered")

	get_tree().paused = true
	print("[GAME OVER] Game paused")

	var root = get_tree().current_scene

	# =========================
	# UI ROOT
	# =========================
	var ui = CanvasLayer.new()
	ui.name = "GameOverUI"
	ui.process_mode = Node.PROCESS_MODE_ALWAYS
	root.add_child(ui)

	print("[UI] CanvasLayer added")

	# =========================
	# DARK OVERLAY
	# =========================
	var overlay = ColorRect.new()
	overlay.color = Color(0, 0, 0, 0.75)
	overlay.anchor_left = 0
	overlay.anchor_top = 0
	overlay.anchor_right = 1
	overlay.anchor_bottom = 1
	overlay.mouse_filter = Control.MOUSE_FILTER_IGNORE
	ui.add_child(overlay)

	print("[UI] Overlay added")

	# =========================
	# CENTER CONTAINER
	# =========================
	var center = CenterContainer.new()
	center.anchor_left = 0
	center.anchor_top = 0
	center.anchor_right = 1
	center.anchor_bottom = 1
	ui.add_child(center)

	var vbox = VBoxContainer.new()
	vbox.add_theme_constant_override("separation", 20)
	center.add_child(vbox)

	# =========================
	# TITLE
	# =========================
	var label = Label.new()
	label.text = "GAME OVER"
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.add_theme_font_size_override("font_size", 64)
	vbox.add_child(label)

	print("[UI] Label added")

	# =========================
	# RESTART BUTTON
	# =========================
	var button = Button.new()
	button.text = "RESTART"
	button.custom_minimum_size = Vector2(220, 70)

	# IMPORTANT for paused UI
	button.process_mode = Node.PROCESS_MODE_ALWAYS
	button.mouse_filter = Control.MOUSE_FILTER_STOP
	button.focus_mode = Control.FOCUS_NONE

	vbox.add_child(button)

	print("[UI] Button added")

	# =========================
	# FIXED SIGNAL (IMPORTANT)
	# =========================
	button.connect("button_up", Callable(self, "_on_restart_pressed"))

	print("[UI] button_up connected")

	# cleanup
	print("[GAME] Cleanup done")


# =========================
# RESTART FUNCTION
# =========================
func _on_restart_pressed():
	print("[RESTART] BUTTON UP FIRED → restarting")

	get_tree().paused = false
	get_tree().reload_current_scene()
