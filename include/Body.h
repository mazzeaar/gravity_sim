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

    // set pressure to std::numeric_limits<double>::max()
    void reset_pressure();

    // adds force to acc (acc += force)
    void add_force(Vec2& force);
    // sets acc to Vec2(0.0, 0.0)
    void reset_force();

    // updates vel and pos based on stored acc and dt
    void update(double dt);
};

#endif // BODY_H

/*
maybe change to struct?
-> object of array instead of array of objects
struct body {
    Vec2 pos*;
    Vec2 vel*;
    Vec2 acc*;

    double mass*;
    double radius*;

    double pressure*;
}
*/