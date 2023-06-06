#include "SimulationManager.h"
#include "Body.h"
#include "Vec2.h"

const double G = 6.67408e-2;
const double theta = 3.0;
const double dt = 0.1;
const unsigned body_count = 10000;

const int width = 2200;
const int height = 2200;

int main()
{
    SimulationManager simulation_manager(width, height, "N-Body Simulation", G, theta);

    std::vector<Body*> bodies;
    double total_mass = 0.0;

    for (int i = 0; i < body_count; ++i)
    {
        double angle = (double) i / body_count * 2 * M_PI;
        Vec2 pos = Vec2(width / 2 + cos(angle) * width / 8, height / 2 + sin(angle) * height / 8);
        Vec2 vel = Vec2(-sin(angle), cos(angle)) * 20;
        double mass = rand() % 5 + 2;
        total_mass += mass;
        bodies.push_back(new Body(pos, vel, mass));
    }

    for (int i = 0; i < body_count; ++i)
    {
        Vec2 pos = Vec2(rand() % (width / 2) + width / 4, rand() % (height / 2) + height / 4);

        // velocity is perpendicular to the vector from the center of the screen to the body, clockwise
        Vec2 vel = Vec2(pos.y - height / 2, width / 2 - pos.x);
        vel = vel / vel.length() * 15;
        vel *= -1.0;

        double mass = rand() % 5 + 1;
        total_mass += mass;
        bodies.push_back(new Body(pos, vel, mass));
    }

    std::cout << "running with " << bodies.size() << " bodies" << std::endl;
    std::cout << "total mass: " << total_mass << std::endl;
    simulation_manager.add_bodys(bodies);

    simulation_manager.start();
}