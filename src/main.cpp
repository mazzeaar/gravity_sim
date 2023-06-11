#include "SimulationManager.h"
#include "Vec2.h"

// ====================================
// ========== CONFIGURATION ===========
// = TODO: make this a config file!!! =
// ====================================
const double G = 6.67408e-4;
const double theta = 1.4;
const double dt = 0.1;

const unsigned body_count = 40000;
const double mass = 100;

const int height = 2200;
const int width = 2200;

// ====================================

int main()
{
    SimulationManager* simulation_manager = new SimulationManager(width, height, "N-Body Simulation", G, theta, dt);

    simulation_manager->set_verbose(false);
    simulation_manager->add_bodies(body_count, mass, BodyType::GALAXY);

    simulation_manager->run();
}