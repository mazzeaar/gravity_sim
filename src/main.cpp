#include "SimulationManager.h"
#include "Vec2.h"

// ====================================
// ========== CONFIGURATION ===========
// = TODO: make this a config file!!! =
// ====================================
const double G = 6.67408e-3;
const double theta = 2.4;
const double dt = 0.1;
const unsigned body_count = 5000;

const int max_mass = 5;

const int height = 2200;
//const int width = height / 19.5 * 9.0;
const int width = height;

// ====================================

// TODO clean up this mess
int main()
{
    SimulationManager simulation_manager(width, height, "N-Body Simulation", G, theta);

    std::vector<Body*> bodies;
    double total_mass = 0.0;

    // rotating circle
    for (int i = 0; i < body_count; ++i)
    {
        double angle = (double) i / body_count * 2 * M_PI;

        Vec2 pos = Vec2(width / 2 + cos(angle) * width / 8, height / 2 + sin(angle) * height / 8);
        Vec2 vel = Vec2(-sin(angle), cos(angle)) * 20;

        double mass = rand() % max_mass + 2;
        total_mass += mass;

        bodies.push_back(new Body(pos, vel, mass));
    }

    // randomly filled square
    for (int i = 0; i < body_count; ++i)
    {
        Vec2 pos = Vec2(rand() % (width / 2) + width / 4, rand() % (height / 2) + height / 4);
        Vec2 vel = Vec2(pos.y - height / 2, width / 2 - pos.x);

        double mass = rand() % max_mass + 1;
        total_mass += mass;

        vel = vel / vel.length() * 50;
        vel *= -1;

        bodies.push_back(new Body(pos, vel, mass));
    }

    std::cout << "running with " << bodies.size() << " bodies" << std::endl;
    std::cout << "total mass: " << total_mass << std::endl;
    simulation_manager.add_bodies(bodies);

    simulation_manager.start();
}