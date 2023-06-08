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
#include <iostream>

class SimulationManager {
private:

    Bodies* bodies;
    Window* window;
    std::shared_ptr<QuadTree> tree;
    std::vector<sf::RectangleShape*> bounding_boxes;

    double G, theta, dt;

    bool toggle_paused;
    bool toggle_draw_quadtree;
    bool toggle_draw_vectors;
    bool toggle_debug;

    bool toggle_verbose;

    long unsigned total_calculations;
    long unsigned calculations_per_frame;

public:
    SimulationManager(const int width, const int height, const char* title = "N-Body Simulation", double G = 6.67408e-11, double theta = 0.8, double dt = 0.1);
    ~SimulationManager();

    void update_simulation(unsigned long& calculations_per_frame);
    void draw_simulation();

    void handle_window_events();
    void print_debug_info(unsigned long steps, double elapsed_time, int calculations_per_frame, double worst_case, double best_case);

    void run();
    inline void pause() { toggle_paused = !toggle_paused; }
    inline void resume() { toggle_paused = !toggle_paused; }
    inline void stop() { this->window->close(); }

    inline void set_G(double G)
    {
        this->G = G;
    }
    inline void set_theta(double theta) { this->theta = theta; }
    inline void set_dt(double dt) { this->dt = dt; }
    inline void set_debug(bool toggle_debug) { this->toggle_debug = toggle_debug; }
    inline void set_draw_vectors(bool toggle_draw_vectors) { this->toggle_draw_vectors = toggle_draw_vectors; }
    inline void set_draw_quadtree(bool toggle_draw_quadtree) { this->toggle_draw_quadtree = toggle_draw_quadtree; }
    inline void set_verbose(bool toggle_verbose) { this->toggle_verbose = toggle_verbose; }

    void add_bodies(unsigned count = 8000, int max_mass = 6);
};

#endif // SIMULATION_MANAGER_H