#include "spider.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/input.hpp>

void godot::Spider::_bind_methods()
{
}

void godot::Spider::_ready()
{
    Ref<Texture2D> texture = ResourceLoader::get_singleton()->load("res://spidey.png");

    if(texture.is_null())
        print_error("Failed to load spider texture!");

    set_texture(texture);
}

void godot::Spider::_process(double delta)
{

}

void godot::SpiderDuo::_bind_methods()
{
}

void godot::SpiderDuo::_ready()
{
    set_process(true);

    pUrsula = memnew(Spider);
    pMartin = memnew(Spider);

    add_child(pUrsula);
    add_child(pMartin);

    Ref<Texture2D> ursulaPNG = ResourceLoader::get_singleton()->load("martin.png");
    Ref<Texture2D> martinPNG = ResourceLoader::get_singleton()->load("ursula.png");

    pUrsula->set_texture(ursulaPNG);
    pMartin->set_texture(martinPNG);

    pUrsula->set_scale({0.3, 0.3});
    pMartin->set_scale({0.3, 0.3});

    rope.resize(rope_segments + 1);

    Vector2 start = pMartin->get_position();
    Vector2 end = pUrsula->get_position();

    RopePoint* r = rope.ptrw();

    for(int i = 0; i <= rope_segments; i++)
    {
        float t = (float)i / rope_segments;
        Vector2 p = start.lerp(end, t);

        r[i].pos = p;
        r[i].prev_pos = p;
    }

    r[0].pinned = true;
    r[rope_segments].pinned = true;
}

void godot::SpiderDuo::_process(double delta)
{
    Input* input = Input::get_singleton();

    Vector2 ursulaVelocity;

    if(input->is_action_pressed("ui_up"))
        ursulaVelocity.y -= 1;
    if(input->is_action_pressed("ui_down"))
        ursulaVelocity.y += 1;
    if(input->is_action_pressed("ui_left"))
        ursulaVelocity.x -= 1;
    if(input->is_action_pressed("ui_right"))
        ursulaVelocity.x += 1;
    
    pUrsula->set_position(pUrsula->get_position() + ursulaVelocity * 200 * delta);

    Vector2 martinVelocity;

    if(input->is_action_pressed("ui_second_up"))
        martinVelocity.y -= 1;
    if(input->is_action_pressed("ui_second_down"))
        martinVelocity.y += 1;
    if(input->is_action_pressed("ui_second_left"))
        martinVelocity.x -= 1;
    if(input->is_action_pressed("ui_second_right"))
        martinVelocity.x += 1;
    
    pMartin->set_position(pMartin->get_position() + martinVelocity * 200 * delta);

    verlet_step(delta);
    solve_constraints();

    queue_redraw();
}

void godot::SpiderDuo::_draw()
{
    if(!pMartin || !pUrsula) return;

    Vector2 posMartin = pMartin->get_position();
    Vector2 posUrsula = pUrsula->get_position();

    if(rope.size() < 2) return;

    for(int i = 0; i < rope.size() - 1; i++)
    {
        draw_line(rope[i].pos, rope[i + 1].pos, Color(1,1,1), 2.0);
    }
}

void godot::SpiderDuo::verlet_step(double delta)
{
    static Vector2 gravity(0, 500.0f);

    for(auto& p : rope)
    {
        if(p.pinned) continue;

        Vector2 temp = p.pos;
        p.pos += (p.pos - p.prev_pos);
        p.prev_pos = temp;
    }
}

void godot::SpiderDuo::solve_constraints()
{
    RopePoint* r = rope.ptrw();
    for(int i = 0; i < 10; i++)
    {
        r[0].pos = pMartin->get_position();
        r[rope_segments].pos = pUrsula->get_position();

        for(int j = 0; j < rope_segments; j++)
        {
            RopePoint &a = rope.write[j];
            RopePoint &b = rope.write[j + 1];

            Vector2 delta = b.pos - a.pos;
            float dist = delta.length();
            float error = dist - (rope_length / rope_segments);

            Vector2 correction = delta.normalized() * (error * 0.5f);

            if(!a.pinned) a.pos += correction;
            if(!b.pinned) b.pos -= correction;
        }
    }
}
