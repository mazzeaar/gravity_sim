#include "Body.h"

Body::Body()
{
    pos = Vec2(0, 0);
    vel = Vec2(0, 0);
    mass = 0;
}

Body::Body(Vec2 position, Vec2 velocity, double mass)
{
    pos = position;
    vel = velocity;
    this->mass = mass;
}

void Body::apply_force(Vec2 force)
{
    acc += force / mass;
}

void Body::reset_force()
{
    acc = Vec2(0.0, 0.0);
}

void Body::update(double dt)
{
    vel += acc * dt;
    pos += vel * dt;
}