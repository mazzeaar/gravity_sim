#include "Body.h"

Body::Body()
{
    this->pos = Vec2(0, 0);
    this->vel = Vec2(0, 0);
    this->mass = 0;
    this->pressure = std::numeric_limits<double>::max();
}

Body::Body(Vec2 position, Vec2 velocity, double mass)
{
    this->pos = position;
    this->vel = velocity;

    this->mass = mass;
    this->radius = pow(mass, 1.0 / 3.0);

    this->pressure = std::numeric_limits<double>::max(); // its not actually pressure, but it gets abused for that
}

void Body::add_force(Vec2& force)
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

double Body::get_pressure()
{
    return this->pressure;
}

void Body::reset_pressure()
{
    this->pressure = std::numeric_limits<double>::max();
}