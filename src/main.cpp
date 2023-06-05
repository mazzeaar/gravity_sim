#include "SimulationManager.h"
#include "Body.h"
#include "Vec2.h"

const double G = 6.67408e-2;
const double theta = 1.4;
const double dt = 0.1;
const unsigned body_count = 8000;

const int width = 2200;
const int height = 2200;

int main()
{
    SimulationManager simulation_manager(width, height, "N-Body Simulation", G, theta);

    std::vector<Body*> bodies;

    // circle
    for (int i = 0; i < body_count; ++i)
    {
        double angle = (double) i / body_count * 2 * M_PI;
        Vec2 pos = Vec2(width / 2 + cos(angle) * width / 4, height / 2 + sin(angle) * height / 4);
        Vec2 vel = Vec2(-sin(angle), cos(angle)) * 20;
        double mass = rand() % 5 + 1;
        bodies.push_back(new Body(pos, vel, mass));
    }

    // smaller circle
    for (int i = 0; i < body_count; ++i)
    {
        double angle = (double) i / body_count * 2 * M_PI;
        Vec2 pos = Vec2(width / 2 + cos(angle) * width / 8, height / 2 + sin(angle) * height / 8);
        Vec2 vel = Vec2(-sin(angle), cos(angle)) * 20;
        double mass = rand() % 5 + 1;
        bodies.push_back(new Body(pos, vel, mass));
    }

    // add one big body in the middle
    bodies.push_back(new Body(Vec2(width / 2, height / 2), Vec2(0, 0), 20));

    /* random
    for (int i = 0; i < body_count; ++i)
    {
         Vec2 pos = Vec2(rand() % width, rand() % height);
         Vec2 vel = Vec2(rand() % 10 - 5, rand() % 10 - 5);
         double mass = rand() % 5 + 1;
         bodies.push_back(new Body(pos, vel, mass));
     }
     */


    simulation_manager.add_bodys(bodies);

    simulation_manager.start();
}