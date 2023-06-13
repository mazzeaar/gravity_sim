#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

class Window;

#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>
#include <iostream>
#include <sstream>

#include "QuadTree.h"
#include "Window.h"
#include "Bodies.h"
#include "ParticleManager.h"

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

    // Debug Info
    long steps;
    long unsigned total_calculations;
    long unsigned calculations_per_frame;

    double average_ratio_best_case;
    double average_ratio_worst_case;

    double calc_best_case;
    double calc_worst_case;

    double elapsed_time_physics;
    double elapsed_time_graphics;
    double total_frame_time;

public:
    SimulationManager(const int width, const int height, const char* title = "N-Body Simulation", double G = 6.67408e-11, double theta = 0.8, double dt = 0.1);
    ~SimulationManager();

    // Getters and Setters
    inline void set_window(Window* window) { this->window = window; }
    inline std::shared_ptr<Bodies> get_bodies() { return bodies; }
    inline std::vector<sf::RectangleShape*> get_bounding_boxes() { return bounding_boxes; }

    inline double get_step() const { return steps; }

    inline double get_G() const { return G; }
    inline double get_theta() const { return theta; }
    inline double get_dt() const { return dt; }

    inline bool get_toggle_paused() const { return paused; }
    inline bool get_toggle_draw_vectors() const { return draw_vectors; }
    inline bool get_toggle_draw_quadtree() const { return draw_quadtree; }
    inline bool get_toggle_debug() const { return debug; }

    inline long unsigned get_calculations_per_frame() const { return calculations_per_frame; }

    // Toggle Functions
    inline void toggle_pause() { paused = !paused; }
    inline void toggle_debug_info() { debug = !debug; }
    inline void toggle_draw_vectors() { draw_vectors = !draw_vectors; }
    inline void toggle_draw_quadtree() { draw_quadtree = !draw_quadtree; }
    inline void toggle_verbose_info() { toggle_verbose = !toggle_verbose; }

    // Setters for Parameters
    inline void increase_G() { this->G *= 2; }
    inline void decrease_G() { this->G = std::max(0.0, this->G * 0.5); }

    inline void increase_theta() { this->theta += 0.1; }
    inline void decrease_theta() { this->theta = std::max(0.0, this->theta - 0.1); }

    inline void increase_dt() { this->dt += 0.05; }
    inline void decrease_dt() { this->dt = std::max(0.05, this->dt - 0.05); }

    // Simulation Functions
    void update_simulation();
    void run();

    // Debug Info Functions
    void print_start_info();
    void print_debug_info();

    std::string get_start_info();
    std::string get_debug_info();

    // Other Functions
    void add_bodies(unsigned count = 8000, double mass = 1.0, BodyType type = BodyType::RANDOM);
    inline sf::VertexArray* get_bounding_rectangles() const { return tree->get_bounding_rectangles(); };

    double get_current_ratio_worst_case();
    double get_current_ratio_best_case();
    double get_average_ratio_best_case();
    double get_average_ratio_worst_case();
    double get_fps();
    double get_elapsed_time_physics();
    double get_elapsed_time_graphics();
    double get_total_frame_time();
    double get_interactions_per_frame();
    double get_total_interactions();
    double get_num_particles();
};

#endif // SIMULATION_MANAGER_H
