#ifndef WINDOW_H
#define WINDOW_H

class SimulationManager;

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "SimulationManager.h"

class Window {
private:
    sf::RenderWindow* window;
    sf::View* view;

    sf::Font font;

    sf::VertexArray calc_per_frame;

    std::shared_ptr<SimulationManager> simulation_manager;
    std::shared_ptr<Bodies> bodies;

    int width, height;

    void draw_bodies();
    void draw_velocity_vectors();
    void draw_quadtree_bounds();
    void draw_start_screen();

    void handle_events();
    void draw_debug();

    void store_png(const std::string& filename);

public:

    Window(int width, int height, const char* title, std::shared_ptr<SimulationManager> simulation_manager);
    ~Window();

    void update();

    bool is_open();

    int get_width();
    int get_height();
};

#endif // WINDOW_H