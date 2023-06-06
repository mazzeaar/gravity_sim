#ifndef BODY_H
#define BODY_H

#include "Vec2.h"
#include <iostream>

class Body {
public:
    Vec2 pos;
    Vec2 vel;
    Vec2 acc;

    double mass;
    double radius;
    double pressure;

    Body();
    Body(Vec2 position, Vec2 velocity, double mass);

    double get_pressure();
    void reset_pressure();

    void apply_force(Vec2 force);
    void reset_force();

    void update(double dt);
};

#endif // BODY_H