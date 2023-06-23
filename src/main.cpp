#include "SimulationManager.h"
#include "Window.h"

#include <fstream>

void readConfig(const std::string& configFile, double& G, double& theta, double& dt, unsigned& body_count, double& mass, int& height, int& width)
{
    std::ifstream file(configFile);
    std::string line;

    while ( std::getline(file, line) )
    {
        if ( line.empty() || line[0] == '#' )
            continue;

        size_t delimiterPos = line.find('=');
        if ( delimiterPos != std::string::npos )
        {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            if ( key == "G" )
                G = std::stod(value);
            else if ( key == "theta" )
                theta = std::stod(value);
            else if ( key == "dt" )
                dt = std::stod(value);
            else if ( key == "body_count" )
                body_count = std::stoul(value);
            else if ( key == "mass" )
                mass = std::stod(value);
            else if ( key == "height" )
                height = std::stoi(value);
            else if ( key == "width" )
                width = std::stoi(value);
        }
    }
}

/*
enum BodyType {
    SPINNING_CIRCLE,
    GALAXY,
    ROTATING_CUBES,
    RANDOM,
    LARGE_CUBE,
    CUSTOM_SHAPE1
};
*/

int main()
{
    double G = 6.67408e-3; // 10e8 stronger gravity
    double theta = 1.2;
    double dt = 0.05;
    unsigned body_count = 100000;
    double mass = 10;
    int height = 2200;
    int width = 2200;

    //readConfig("../CONFIG.cfg", G, theta, dt, body_count, mass, height, width);
    SimulationManager* simulation_manager = new SimulationManager(width, height, "N-Body Simulation", G, theta, dt);

    simulation_manager->add_bodies(body_count, mass, BodyType::RANDOM);
    simulation_manager->toggle_debug_info();
    simulation_manager->toggle_pause();

    Window* window = new Window(width, height, "N-Body Simulation", std::unique_ptr<SimulationManager>(simulation_manager));
    simulation_manager->set_window(window);

    simulation_manager->run();
}