#include "SimulationManager.h"
#include "Body.h"
#include "Vec2.h"

const double G = 6.67408e-2;
const double theta = 0.8;
const double dt = 0.1;
const unsigned body_count = 10000;

const int width = 2000;
const int height = 2000;

int main()
{
    SimulationManager simulation_manager(width, height, "N-Body Simulation", G, theta);

    std::vector<Body*> bodies;

    for (int i = 0; i < body_count; ++i)
    {
        double angle = (double) i / body_count * 2 * M_PI;
        Vec2 pos = Vec2(width / 2 + cos(angle) * width / 4, height / 2 + sin(angle) * height / 4);
        Vec2 vel = Vec2(0, 0);
        double mass = rand() % 5 + 1;
        bodies.push_back(new Body(pos, vel, mass));
    }

    simulation_manager.add_bodys(bodies);

    simulation_manager.start();
}