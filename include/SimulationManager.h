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

    // Simulation Settings
    double G, theta, dt;

    // Simulation Toggles
    bool paused;
    bool draw_quadtree;
    bool draw_vectors;
    bool debug;
    bool toggle_verbose;

    // Simulation Stats
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

    void update_simulation();
    void run();

    /*--------------------
    |   Member Setters   |
    ---------------------*/
    inline void set_window(Window* window) { this->window = window; }
    inline void add_bodies(unsigned count = 8000, double mass = 1.0, BodyType body_type = BodyType::RANDOM) { particle_manager->add_bodies(body_type, count, mass); }

    /*--------------------
    |   Member Getters   |
    ---------------------*/
    inline std::shared_ptr<Bodies> get_bodies() { return bodies; }
    inline std::vector<sf::RectangleShape*> get_bounding_boxes() { return bounding_boxes; }
    inline sf::VertexArray* get_bounding_rectangles() const { return tree->get_bounding_rectangles(); };

    /*--------------------
    | Simulation Settings |
    ---------------------*/
    void reset_simulation();

    inline void increase_G() { this->G *= 2; }
    inline void decrease_G() { this->G = std::max(0.0, this->G * 0.5); }

    inline void increase_theta() { this->theta += 0.1; }
    inline void decrease_theta() { this->theta = std::max(0.0, this->theta - 0.1); }

    inline void increase_dt() { this->dt += 0.05; }
    inline void decrease_dt() { this->dt = std::max(0.05, this->dt - 0.05); }

    inline long get_step() const { return steps; }

    inline double get_G() const { return G; }
    inline double get_theta() const { return theta; }
    inline double get_dt() const { return dt; }

    inline Vec2 get_center_of_mass() const { return tree->get_center_of_mass(); }
    inline void get_quadtree_size(double& x, double& y, double& width, double& height) const
    {
        Vec2 top_left, bottom_right;
        tree->get_size(top_left, bottom_right);
        x = top_left.x;
        y = top_left.y;
        width = bottom_right.x - top_left.x;
        height = bottom_right.y - top_left.y;
    }

    /*--------------------
    | Simulation Toggles |
    ---------------------*/
    inline void toggle_pause() { paused = !paused; }
    inline void toggle_debug_info() { debug = !debug; }
    inline void toggle_draw_vectors() { draw_vectors = !draw_vectors; }
    inline void toggle_draw_quadtree() { draw_quadtree = !draw_quadtree; }
    inline void toggle_verbose_info() { toggle_verbose = !toggle_verbose; }

    /*--------------------
    |   Toggle Getters   |
    ---------------------*/
    inline bool get_toggle_paused() const { return paused; }
    inline bool get_toggle_draw_vectors() const { return draw_vectors; }
    inline bool get_toggle_draw_quadtree() const { return draw_quadtree; }
    inline bool get_toggle_debug() const { return debug; }

    /*--------------------
    |  Sim Stat Getters  |
    ---------------------*/
    double get_current_ratio_worst_case();
    double get_current_ratio_best_case();
    double get_average_ratio_best_case();
    double get_average_ratio_worst_case();

    inline double get_fps() const { return 1e6 * 1.0 / this->total_frame_time; }
    inline double get_num_particles() const { return static_cast<double>(bodies->get_size()); }

    inline double get_elapsed_time_physics() const { return this->elapsed_time_physics / 1000; }
    inline double get_elapsed_time_graphics() const { return this->elapsed_time_graphics / 1000; }
    inline double get_total_frame_time() const { return this->total_frame_time / 1000; }

    inline double get_interactions_per_frame() const { return static_cast<double>(this->calculations_per_frame); }
    inline double get_total_interactions() const { return static_cast<double>(this->total_calculations); }
};

#endif // SIMULATION_MANAGER_H
