#include "spider.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>

void godot::Spider::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("touchesRope", "obj"), &SpiderDuo::touchesRope);
}

void godot::Spider::_ready()
{
    
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
    set_name("SpiderDuo");
    
    pUrsula = memnew(Spider);
    pUrsula->set_name("Ursula");

    pMartin = memnew(Spider);
    pMartin->set_name("Martin");

    add_child(pUrsula);
    add_child(pMartin);

    Ref<Texture2D> ursulaPNG = ResourceLoader::get_singleton()->load("ursula.png");
    Ref<Texture2D> martinPNG = ResourceLoader::get_singleton()->load("martin.png");

    pUrsula->set_texture(ursulaPNG);
    pMartin->set_texture(martinPNG);

    pUrsula->set_scale({0.5, 0.5});
    pMartin->set_scale({0.5, 0.5});

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
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Node *tilemap_node = get_parent()->get_node_or_null("Collisions");
    TileMapLayer *tilemap_layer = Object::cast_to<TileMapLayer>(tilemap_node);

    Input *input = Input::get_singleton();

    Vector2 ursula_dir;

    if (input->is_action_pressed("ui_up")) ursula_dir.y -= 1;
    if (input->is_action_pressed("ui_down")) ursula_dir.y += 1;
    if (input->is_action_pressed("ui_left")) ursula_dir.x -= 1;
    if (input->is_action_pressed("ui_right")) ursula_dir.x += 1;

    if (ursula_dir != Vector2())
        ursula_dir = ursula_dir.normalized();

    Vector2 martin_dir;

    if (input->is_action_pressed("ui_second_up")) martin_dir.y -= 1;
    if (input->is_action_pressed("ui_second_down")) martin_dir.y += 1;
    if (input->is_action_pressed("ui_second_left")) martin_dir.x -= 1;
    if (input->is_action_pressed("ui_second_right")) martin_dir.x += 1;

    if (martin_dir != Vector2())
        martin_dir = martin_dir.normalized();



    Vector2 ursula_pos = pUrsula->get_position();

    Vector2 ursula_try_x = ursula_pos + Vector2(ursula_dir.x * 200.0f * delta, 0);
    Vector2 ursula_local_x = tilemap_layer->to_local(ursula_try_x);
    Vector2i cell_x = tilemap_layer->local_to_map(ursula_local_x);

    if (tilemap_layer->get_cell_source_id(cell_x) == -1)
        ursula_pos.x = ursula_try_x.x;

    Vector2 ursula_try_y = ursula_pos + Vector2(0, ursula_dir.y * 200.0f * delta);
    Vector2 ursula_local_y = tilemap_layer->to_local(ursula_try_y);
    Vector2i cell_y = tilemap_layer->local_to_map(ursula_local_y);

    if (tilemap_layer->get_cell_source_id(cell_y) == -1)
        ursula_pos.y = ursula_try_y.y;

    pUrsula->set_position(ursula_pos);



    Vector2 martin_pos = pMartin->get_position();

    Vector2 martin_try_x = martin_pos + Vector2(martin_dir.x * 200.0f * delta, 0);
    Vector2 martin_local_x = tilemap_layer->to_local(martin_try_x);
    Vector2i martin_cell_x = tilemap_layer->local_to_map(martin_local_x);

    if (tilemap_layer->get_cell_source_id(martin_cell_x) == -1)
        martin_pos.x = martin_try_x.x;

    Vector2 martin_try_y = martin_pos + Vector2(0, martin_dir.y * 200.0f * delta);
    Vector2 martin_local_y = tilemap_layer->to_local(martin_try_y);
    Vector2i martin_cell_y = tilemap_layer->local_to_map(martin_local_y);

    if (tilemap_layer->get_cell_source_id(martin_cell_y) == -1)
        martin_pos.y = martin_try_y.y;

    pMartin->set_position(martin_pos);

    Vector2 deltaVec = pUrsula->get_position() - pMartin->get_position();
    float dist = deltaVec.length();

    if(dist > max_distance)
    {
        Vector2 dir = deltaVec / dist;
        float excess = dist - max_distance;

        Vector2 correction = dir * (excess * 0.5f);

        pMartin->set_position(pMartin->get_position() + correction);
        pUrsula->set_position(pUrsula->get_position() - correction);

        is_tense = true;
    } else is_tense = false;

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

    Color col;
    if(is_tense)
        col = Color(1,0,0);
    else col = Color(1,1,1);

    for(int i = 0; i < rope.size() - 1; i++)
    {
        draw_line(rope[i].pos, rope[i + 1].pos, col, 1.0);
    }

    float health_ratio = mHealthPoints / 100.0f;

    Vector2 bar_pos = posMartin + Vector2(-10, -8);
    Vector2 bar_size = Vector2(20, 2);

    draw_rect(Rect2(bar_pos, bar_size), Color(0.2, 0.2, 0.2));

    draw_rect(
        Rect2(bar_pos, Vector2(bar_size.x * health_ratio, bar_size.y)),
        Color(0, 1, 0)
    );

    bar_pos = posUrsula + Vector2(-10, -8);
        draw_rect(Rect2(bar_pos, bar_size), Color(0.2, 0.2, 0.2));

    draw_rect(
        Rect2(bar_pos, Vector2(bar_size.x * health_ratio, bar_size.y)),
        Color(0, 1, 0)
    );
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

bool godot::SpiderDuo::touchesRope(Node2D *obj)
{
    const RopePoint* r = rope.ptr();
    Vector2 p = to_local(obj->get_global_position());
    float radius = 10.0f;

    for (int i = 0; i < rope_segments - 1; i++)
    {
        Vector2 a = r[i].pos;
        Vector2 b = r[i + 1].pos;

        Vector2 ab = b - a;
        Vector2 ap = p - a;

        float denom = ab.length_squared();
        if (denom < 0.000001f)
            continue;

        float t = ap.dot(ab) / denom;
        t = Math::clamp(t, 0.0f, 1.0f);

        Vector2 closest = a + ab * t;

        if ((p - closest).length() <= radius)
        {
            return true;
        }
    }

    return false;
}

bool godot::SpiderDuo::isTense() const
{
    return is_tense;
}

void godot::SpiderDuo::takeDamage(int amount)
{
    mHealthPoints -= amount;
    if(mHealthPoints <= 0)
        queue_free();
}

bool godot::SpiderDuo::collidesSpiders(Vector2 point)
{
    Vector2 ursulaPos = pUrsula->get_position();
    Vector2 martinPos = pMartin->get_position();

    Vector2 ursulaVec = point - ursulaPos;
    Vector2 martinVec = point - martinPos;

    float ursulaDist = ursulaVec.x * ursulaVec.x + ursulaVec.y * ursulaVec.y;
    float martinDist = martinVec.x * martinVec.x + martinVec.y * martinVec.y;

    if(ursulaDist < 400.0f || martinDist < 400.0f)
        return true;

    return false;
}

void godot::Enemy::_bind_methods()
{
}

void godot::Enemy::_ready()
{
    Ref<Texture2D> source = ResourceLoader::get_singleton()->load("zuk.png");

    set_texture(source);

    set_global_position({500.0f, 500.0f});
    spidersNode = get_parent()->get_node_or_null("SpiderDuo");
    spiderNode = spidersNode->get_node_or_null("Ursula");


}

void godot::Enemy::_process(double delta)
{
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    Spider* spider = Object::cast_to<Spider>(spiderNode);
    SpiderDuo* spiderDuo = Object::cast_to<SpiderDuo>(spidersNode);

    if(!spider || !spiderDuo) return;

    Vector2 spiderPos = spider->get_global_position();
    Vector2 bugPos = get_global_position();

    Vector2 unitVec = (spiderPos - bugPos).normalized();

    set_global_position(bugPos + unitVec * 100.0 * delta);

    if(spiderDuo->touchesRope(this) && spiderDuo->isTense())
    {
        queue_free();
        return;
    }

    static double lastDamageTime = -1000.0;
    double cooldown = 2.0;
    if(spiderDuo->collidesSpiders(get_position()))
    {
        double now = Time::get_singleton()->get_ticks_msec() / 1000.0;

        if(now - lastDamageTime < cooldown)
            return;
        
        lastDamageTime = now;

        spiderDuo->takeDamage(20);
    }
}
