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
    SimulationManager* simulation_manager = new SimulationManager(width, height, "N-Body Simulation", G, theta, dt);

    simulation_manager->set_debug(true);
    simulation_manager->set_draw_vectors(false);
    simulation_manager->set_draw_quadtree(false);
    simulation_manager->add_bodies(body_count, max_mass);
    std::cout << "body count: " << body_count << std::endl;
    simulation_manager->run();
}