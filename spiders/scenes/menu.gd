extends Control

@onready var main_buttons: VBoxContainer = $MainButtons
@onready var options: Panel = $Options

@export var audio_bus_name: String = "Master"

const MIN_FPS := 15
const MAX_FPS := 60

func _ready():
	main_buttons.visible = true
	options.visible = false

	Engine.max_fps = MAX_FPS

	var bus_index = AudioServer.get_bus_index(audio_bus_name)
	AudioServer.set_bus_volume_db(bus_index, 0)


func _on_start_pressed() -> void:
	get_tree().change_scene_to_file("uid://eyhwu4dxxrck")


func _on_options_pressed() -> void:
	main_buttons.visible = false
	options.visible = true


func _on_quit_pressed() -> void:
	get_tree().quit()


func _on_back_pressed() -> void:
	main_buttons.visible = true
	options.visible = false


func _on_save_pressed() -> void:
	print("Ustawienia zapisane")


func _on_fps_value_changed(value: int) -> void:
	var clamped_fps = clamp(value, MIN_FPS, MAX_FPS)
	Engine.max_fps = clamped_fps
	print("FPS ustawione na:", clamped_fps)


func _on_volume_value_changed(value: float) -> void:
	var bus_index = AudioServer.get_bus_index(audio_bus_name)

	value = clamp(value, 0.0, 1.0)

	AudioServer.set_bus_volume_db(bus_index, linear_to_db(value))
	print("Głośność:", value)
