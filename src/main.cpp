#include "SimulationManager.h"
#include "Vec2.h"

// ====================================
// ========== CONFIGURATION ===========
// = TODO: make this a config file!!! =
// ====================================
const double G = 6.67408e0;
const double theta = 0.6;
const double dt = 0.1;
const unsigned body_count = 5000;

const int max_mass = 100;

const int height = 2200;
//const int width = height / 19.5 * 9.0;
const int width = height;

// ====================================

// TODO clean up this mess
int main()
{
    SimulationManager* simulation_manager = new SimulationManager(width, height, "N-Body Simulation", G, theta, dt);

    simulation_manager->set_debug(false);
    simulation_manager->set_verbose(false);

    simulation_manager->set_draw_vectors(false);
    simulation_manager->set_draw_quadtree(false);

    simulation_manager->add_bodies(10000, max_mass);

    simulation_manager->run();
}