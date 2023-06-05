#include "SimulationManager.h"

SimulationManager::SimulationManager(int width, int height, const char* title, double G, double theta, double dt)
{
    this->window = new Window(width, height, title);
    this->G = G;
    this->theta = theta;
    this->dt = dt;

    this->paused = false;
    this->draw_quadtree = false;

    // Set the boundaries of the QuadTree based on the window size
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = static_cast<double>(width);
    double ymax = static_cast<double>(height);

    this->tree = new QuadTree(xmin, ymin, xmax, ymax);
}


SimulationManager::~SimulationManager()
{
    delete this->window;
    delete this->tree;
}

void SimulationManager::start()
{
    while (this->window->is_open())
    {
        handle_window_events();

        if (!paused)
        {
            update_simulation();
        }

        draw_simulation();
    }
}

void SimulationManager::stop()
{
    this->window->close();
}

void SimulationManager::pause()
{
    this->paused = true;
}

void SimulationManager::resume()
{
    this->paused = false;
}

void SimulationManager::set_G(double G)
{
    this->G = G;
}

void SimulationManager::set_theta(double theta)
{
    this->theta = theta;
}

void SimulationManager::add_body_at_position(Vec2 position, Vec2 velocity, double mass)
{
    Body* new_body = new Body(position, velocity, mass);
    this->bodies.push_back(new_body);
    this->tree->insert(new_body);
}

void SimulationManager::add_body(Body* body)
{
    this->bodies.push_back(body);
    this->tree->insert(body);
}

void SimulationManager::add_bodys(std::vector<Body*> bodys)
{
    for (Body* body : bodys)
    {
        this->add_body(body);
    }
}

void SimulationManager::update_simulation()
{
    this->tree->add_bodys(this->bodies);
    this->tree->update(this->bodies, this->theta, this->G, this->dt);

    for (Body* body : this->bodies)
    {
        body->pos += body->vel * this->dt;
    }
}

void SimulationManager::draw_simulation()
{
    this->window->clear();

    if (this->draw_quadtree)
    {
        this->tree->get_bounding_rectangles(this->bounding_boxes);

        for (sf::RectangleShape* rectangle : bounding_boxes)
        {
            this->window->draw(rectangle);
        }
    }

#pragma omp parallel for
    for (Body* body : this->bodies)
    {
        this->window->circle->setRadius(body->mass);
        this->window->circle->setPosition(body->pos.x - body->mass, body->pos.y - body->mass);
        this->window->circle->setFillColor(sf::Color::White);
        this->window->draw(this->window->circle);
    }

    this->window->display();

    delete this->tree;
    this->tree = new QuadTree(0.0, 0.0, (double) this->window->get_width(), (double) this->window->get_height());
}

void SimulationManager::handle_window_events()
{
    this->window->handle_events(this->paused, this->draw_quadtree);
}