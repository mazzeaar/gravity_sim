#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "QuadTree.h"
#include "Body.h"
#include "Window.h"
#include "Vec2.h"

#include <SFML/Graphics.hpp>
#include <vector>

class SimulationManager {
private:
    std::vector<Body*> bodies;
    std::vector<sf::RectangleShape*> bounding_boxes;

    QuadTree* tree;
    double G, theta, dt;

    bool paused;
    bool draw_quadtree;
    bool draw_vectors;

    Window* window;

public:
    SimulationManager(int width, int height, const char* title, double G, double theta, double dt = 0.1);
    ~SimulationManager();

    void start();
    void pause();
    void resume();
    void stop();

    void update_simulation();
    void draw_simulation();

    void set_G(double G);
    void set_theta(double theta);

    void add_body(Body* body);
    void add_bodys(std::vector<Body*> bodys);
    void add_body_at_position(Vec2 position, Vec2 velocity, double mass);

    void handle_window_events();
};

#endif // SIMULATION_MANAGER_H