#include "spider.hpp"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/input.hpp>

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

    pUrsula->set_scale({2, 2});
    pMartin->set_scale({2, 2});

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
        draw_line(rope[i].pos, rope[i + 1].pos, col, 2.0);
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
}
