#include "SimulationManager.h"
#include "Vec2.h"

// ====================================
// ========== CONFIGURATION ===========
// = TODO: make this a config file!!! =
// ====================================
const double G = 6.67408e-3;
const double theta = 1.8;
const double dt = 0.1;
const unsigned body_count = 10000;

const int max_mass = 10;

const int height = 2200;
//const int width = height / 19.5 * 9.0;
const int width = height;

// ====================================

int main()
{
    SimulationManager* simulation_manager = new SimulationManager(width, height, "N-Body Simulation", G, theta, dt);

    simulation_manager->set_debug(false);
    simulation_manager->set_verbose(false);

    simulation_manager->set_draw_vectors(false);
    simulation_manager->set_draw_quadtree(false);

    simulation_manager->add_bodies(body_count, max_mass);

    simulation_manager->run();
}