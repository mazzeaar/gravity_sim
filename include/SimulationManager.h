#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "QuadTree.h"
#include "Window.h"
#include "Vec2.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>

class SimulationManager {
private:
    Bodies bodies;
    std::vector<sf::RectangleShape*> bounding_boxes;

    QuadTree* tree;
    double G, theta, dt;

    bool toggle_paused;
    bool draw_quadtree;
    bool draw_vectors;
    bool debug;

    long long unsigned total_calculations;
    long unsigned calculations_per_frame;

    Window* window;

public:
    SimulationManager(int width, int height, const char* title, double G, double theta, double dt = 0.1);
    ~SimulationManager();

    void start();

    void update_simulation(unsigned long& calculations_per_frame);
    void draw_simulation();

    void handle_window_events();
    void print_debug_info(unsigned long steps, double elapsed_time, int calculations_per_frame, double worst_case, double best_case);

    inline void pause() { toggle_paused = !toggle_paused; }
    inline void resume() { toggle_paused = !toggle_paused; }
    inline void stop() { this->window->close(); }
    inline void set_G(double G) { this->G = G; }
    inline void set_theta(double theta) { this->theta = theta; }
};

#endif // SIMULATION_MANAGER_H