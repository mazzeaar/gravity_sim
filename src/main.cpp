#include "SimulationManager.h"
#include "Window.h"
#include "Vec2.h"

// ====================================
// ========== CONFIGURATION ===========
// = TODO: make this a config file!!! =
// ====================================
const double G = 6.67408e-3; // 10e8 stronger gravity
const double theta = 1.4;
const double dt = 0.1;

const unsigned body_count = 69420;
const double mass = 10;

const int height = 2200;
const int width = 2200;

// ====================================

int main()
{
    SimulationManager* simulation_manager = new SimulationManager(width, height, "N-Body Simulation", G, theta, dt);

    simulation_manager->add_bodies(body_count, mass, BodyType::SPINNING_CIRCLE);

    Window* window = new Window(width, height, "N-Body Simulation", std::unique_ptr<SimulationManager>(simulation_manager));
    simulation_manager->set_window(window);

    simulation_manager->run();
}