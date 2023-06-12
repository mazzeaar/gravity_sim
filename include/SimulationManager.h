#pragma once

class Window;

#include "QuadTree.h"
#include "Window.h"
#include "Bodies.h"
#include "ParticleManager.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>
#include <iostream>
#include <sstream>

class SimulationManager {
private:

    std::shared_ptr<Bodies> bodies;
    Window* window;

    std::shared_ptr<QuadTree> tree;
    std::shared_ptr<ParticleManager> particle_manager;

    std::vector<sf::RectangleShape*> bounding_boxes;

    double G, theta, dt;

    bool paused;
    bool draw_quadtree;
    bool draw_vectors;
    bool debug;

    bool toggle_verbose;

    // debug info stuff
    long steps;
    long unsigned total_calculations;
    long unsigned calculations_per_frame;

    double elapsed_time_physics;
    double elapsed_time_graphics;
    double total_frame_time;

public:
    SimulationManager(const int width, const int height, const char* title = "N-Body Simulation", double G = 6.67408e-11, double theta = 0.8, double dt = 0.1);
    ~SimulationManager();

    inline void set_window(Window* window) { this->window = window; }

    void update_simulation(unsigned long& calculations_per_frame);

    void print_start_info();
    void print_debug_info();
    std::string get_debug_info();

    void run();
    inline void toggle_pause() { paused = !paused; }

    inline std::shared_ptr<Bodies> get_bodies() { return bodies; }
    inline std::vector<sf::RectangleShape*> get_bounding_boxes() { return bounding_boxes; }

    inline void toggle_debug_info() { this->debug = !this->debug; }
    inline void toggle_draw_vectors() { this->draw_vectors = !this->draw_vectors; }
    inline void toggle_draw_quadtree() { this->draw_quadtree = !this->draw_quadtree; }
    inline void toggle_verbose_info() { this->toggle_verbose = !this->toggle_verbose; }

    inline bool get_toggle_paused() { return paused; }
    inline bool get_toggle_draw_vectors() { return draw_vectors; }
    inline bool get_toggle_draw_quadtree() { return draw_quadtree; }
    inline bool get_toggle_debug() { return debug; }

    inline void set_G(double G) { this->G = G; }
    inline void increase_G() { this->G += 0.1; }
    inline void decrease_G() { this->G = std::max(0.0, this->G - 0.1); }
    inline double get_G() const { return G; }


    inline void set_theta(double theta) { this->theta = theta; }
    inline void increase_theta() { this->theta += 0.1; }
    inline void decrease_theta() { this->theta = std::max(0.0, this->theta - 0.1); }
    inline double get_theta() const { return theta; }

    inline void set_dt(double dt) { this->dt = dt; }
    inline void increase_dt() { this->dt += 0.05; }
    inline void decrease_dt() { this->dt = std::max(0.05, this->dt - 0.05); }
    inline double get_dt() const { return dt; }

    void add_bodies(unsigned count = 8000, double mass = 1.0, BodyType type = BodyType::RANDOM);
    inline sf::VertexArray* get_bounding_rectangles() const { return tree->get_bounding_rectangles(); };
};

