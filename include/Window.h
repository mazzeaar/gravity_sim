#pragma once

class SimulationManager;

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "SimulationManager.h"

class Window {
private:
    sf::RenderWindow* window;
    sf::View* view;

    std::shared_ptr<SimulationManager> simulation_manager;
    std::shared_ptr<Bodies> bodies;

    int width, height;

    void draw_bodies();
    void draw_velocity_vectors();
    void draw_quadtree_bounds();

    void handle_events();

public:
    bool paused;

    Window(int width, int height, const char* title, std::shared_ptr<SimulationManager> simulation_manager);
    ~Window();

    void update();

    void store_png(const std::string& filename);

    bool is_open();

    int get_width();
    int get_height();
};