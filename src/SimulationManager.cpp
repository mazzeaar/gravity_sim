#include "SimulationManager.h"

SimulationManager::SimulationManager(int width, int height, const char* title, double G, double theta, double dt) {
    this->window = new Window(width, height, title);
    this->G = G;
    this->theta = theta;
    this->dt = dt;

    // Set the boundaries of the QuadTree based on the window size
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = static_cast<double>(width);
    double ymax = static_cast<double>(height);

    this->tree = new QuadTree(xmin, ymin, xmax, ymax);
}


SimulationManager::~SimulationManager() {
    delete this->window;
    delete this->tree;
}

void SimulationManager::start() {
    while (this->window->is_open()) {

        this->window->handle_events();

        this->window->clear();

        this->tree->add_bodys(this->bodies);
        this->tree->update(this->bodies, this->theta, this->G, this->dt);

        for (Body* body : this->bodies) {
            this->window->circle->setRadius(body->mass);
            this->window->circle->setPosition(body->pos.x - body->mass, body->pos.y - body->mass);
            this->window->circle->setFillColor(sf::Color::White);
            this->window->draw(this->window->circle);
        }

        std::vector<sf::RectangleShape> rectangles = this->tree->getBoundingRectangles();
        for (sf::RectangleShape rect : rectangles) {
            this->window->draw(&rect);
        }

        this->window->display();

        delete this->tree;
        this->tree = new QuadTree(0.0, 0.0, (double) this->window->get_width(), (double) this->window->get_height());

    }
}

void SimulationManager::stop() {
    this->window->close();
}

void SimulationManager::set_G(double G) {
    this->G = G;
}

void SimulationManager::set_theta(double theta) {
    this->theta = theta;
}

void SimulationManager::add_body(Body* body) {
    this->bodies.push_back(body);
    this->tree->insert(body);
}

void SimulationManager::add_bodys(std::vector<Body*> bodys) {
    for (Body* body : bodys) {
        this->add_body(body);
    }
}

void SimulationManager::update() {
    this->tree->add_bodys(this->bodies);
    this->tree->update(this->bodies, this->theta, this->G, this->dt);

    for (Body* body : this->bodies) {
        // Update the position based on the calculated forces and time step
        body->pos += body->vel * this->dt;

        // Draw the body on the window
        this->window->circle->setRadius(body->mass);
        this->window->circle->setPosition(body->pos.x - body->mass, body->pos.y - body->mass);
        this->window->circle->setFillColor(sf::Color::White);
        this->window->draw(this->window->circle);
    }

    // Clear the tree and create a new one
    delete this->tree;
    this->tree = new QuadTree(0.0, 0.0, (double) this->window->get_width(), (double) this->window->get_height());
}
