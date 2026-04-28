extends Camera2D


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	global_position = $"../SpiderDuo".get_child(0).global_position
