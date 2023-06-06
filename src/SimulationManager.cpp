#include "SimulationManager.h"

SimulationManager::SimulationManager(int width, int height, const char* title, double G, double theta, double dt)
{
    this->window = new Window(width, height, title);
    this->G = G;
    this->theta = theta;
    this->dt = dt;

    this->paused = false;
    this->draw_quadtree = false;

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
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        handle_window_events();

        if (!paused)
        {
            update_simulation();
        }

        draw_simulation();

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0;

        std::cout << std::left << std::setw(4) << std::setprecision(3) << 1e3 * 1.0 / elapsed_time << " fps | ";
        std::cout << std::left << std::setw(4) << std::setprecision(3) << (elapsed_time / 1e3) << " spf\n";
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

void SimulationManager::set_G(/*SNOOP D-O-*/ double G)
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
        this->bounding_boxes.clear();
        this->tree->get_bounding_rectangles(this->bounding_boxes);

        for (sf::RectangleShape* rectangle : bounding_boxes)
        {
            this->window->draw(rectangle);
        }
    }

    if (this->draw_vectors)
    {
        for (Body* body : this->bodies)
        {
            double angle = atan2(body->vel.y, body->vel.x);
            double length = body->vel.length();

            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(body->pos.x, body->pos.y)),
                sf::Vertex(sf::Vector2f(body->pos.x + cos(angle) * length, body->pos.y + sin(angle) * length))
            };

            this->window->draw(line, 2, sf::Lines);
        }
    }

    double lowest_pressure = std::numeric_limits<double>::max();
    double highest_pressure = std::numeric_limits<double>::min();

    for (Body* body : this->bodies)
    {
        double pressure = body->get_pressure();
        lowest_pressure = std::min(lowest_pressure, pressure);
        highest_pressure = std::max(highest_pressure, pressure);
    }

    for (Body* body : this->bodies)
    {
        this->window->circle->setRadius(body->radius);
        this->window->circle->setPosition(body->pos.x - body->radius, body->pos.y - body->radius);

        double normalized_pressure = (body->get_pressure() - lowest_pressure) / (highest_pressure - lowest_pressure);

        sf::Color startColor(100, 100, 255);
        sf::Color endColor(255, 0, 0);

        sf::Color interpolatedColor(
            static_cast<sf::Uint8>((1 - normalized_pressure) * startColor.r + normalized_pressure * endColor.r),
            static_cast<sf::Uint8>((1 - normalized_pressure) * startColor.g + normalized_pressure * endColor.g),
            static_cast<sf::Uint8>((1 - normalized_pressure) * startColor.b + normalized_pressure * endColor.b)
        );

        // interpolatedColor.a = (1 - normalized_pressure) * 100 + 155;
        interpolatedColor.a = 222;

        this->window->circle->setFillColor(interpolatedColor);
        this->window->draw(this->window->circle);
        body->reset_pressure();
    }

    this->window->display();
    this->tree->clear();
}

void SimulationManager::handle_window_events()
{
    this->window->handle_events(this->paused, this->draw_quadtree, this->draw_vectors);
}