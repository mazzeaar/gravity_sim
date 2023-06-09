#ifndef PARTICLE_MANAGER_H
#define PARTICLE_MANAGER_H

#include "Bodies.h"

#include <random>

enum BodyType {
    SPINNING_CIRCLE,
    GALAXY,
    ROTATING_CUBES,
    RANDOM,
    LARGE_CUBE,
    CUSTOM_SHAPE1
};

class ParticleManager {
private:
    std::shared_ptr<Bodies> bodies;
    enum BodyType body_type;
    double mass;
    unsigned width, height;

    void add_spinning_circle(unsigned num_bodies, double mass);
    void add_galaxy(unsigned num_bodies, double mass);
    void add_rotating_cubes(unsigned num_bodies, double mass);
    void add_random(unsigned num_bodies, double mass);
    void add_large_cube(unsigned num_bodies, double mass);
    void add_custom_shape1(unsigned count, double mass);

public:
    ParticleManager(std::shared_ptr<Bodies> bodies, unsigned width, unsigned height);
    ~ParticleManager();

    void add_bodies(BodyType type = BodyType::GALAXY, unsigned num_bodies = 20000, double mass = 1.0);
    void get_particle_area(Vec2& top_left, Vec2& bottom_right);
    void reset();
};

#endif // PARTICLE_MANAGER_H