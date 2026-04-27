extends Node2D
var spider_duo_scene = preload("res://scenes/spider_duo.tscn")

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var spider_duo = spider_duo_scene.instantiate()
	add_child(spider_duo)
	spider_duo.position = Vector2(0, 0)
	spider_duo.scale = Vector2(0.2, 0.2)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
