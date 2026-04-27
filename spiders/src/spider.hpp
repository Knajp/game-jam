#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

namespace godot
{
    struct RopePoint
    {
        Vector2 pos;
        Vector2 prev_pos;
        bool pinned = false;
    };
    class Spider : public Sprite2D
    {
        GDCLASS(Spider, Sprite2D)
    protected:
        static void _bind_methods();
    public:
        void _ready();
        void _process(double delta);
    };

    class SpiderDuo : public Node2D
    {
        GDCLASS(SpiderDuo, Node2D)
    protected:
        static void _bind_methods();
    private:
        Spider* pMartin = nullptr;
        Spider* pUrsula = nullptr;

        Vector<RopePoint> rope;
        int rope_segments = 20;
        float rope_length = 200.0f;

        float max_distance = 300.0f;
        bool is_tense = false;

        int mHealthPoints = 100;
    public:
        void _ready();
        void _process(double delta);
        void _draw();

        void verlet_step(double delta);
        void solve_constraints();

        bool touchesRope(Node2D* obj);
        bool isTense() const;
        void takeDamage(int amount);

        bool collidesSpiders(Vector2 point);

    };


    class Enemy : public Sprite2D
    {
        GDCLASS(Enemy, Sprite2D)
    protected:
        static void _bind_methods();
    public:
        void _ready();
        void _process(double delta);

    private:
        Node* spiderNode = nullptr;
        Node* spidersNode = nullptr;
    };

}