#include "SimulationManager.h"

SimulationManager::SimulationManager(int width, int height, const char* title, double G, double theta, double dt)
    : G(G), theta(theta), dt(dt), toggle_paused(false), draw_quadtree(false), draw_vectors(false), debug(false), total_calculations(0)
{
    window = new Window(width, height, title);
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = static_cast<double>(width);
    double ymax = static_cast<double>(height);
    tree = new QuadTree(xmin, ymin, xmax, ymax, &bodies);
}

SimulationManager::~SimulationManager()
{
    delete window;
    delete tree;
}

void SimulationManager::start()
{
    double worst_case = bodies.size() * bodies.size();
    double best_case = bodies.size() * log2(bodies.size());
    unsigned long steps = 0;

    while (window->is_open())
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        handle_window_events();

        if (!toggle_paused)
        {
            update_simulation(calculations_per_frame);

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            total_calculations += calculations_per_frame;
            if (debug)
            {
                print_debug_info(steps, std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0, calculations_per_frame, worst_case, best_case);
            }
            calculations_per_frame = 0;
        }
        draw_simulation();
    }
}

void SimulationManager::update_simulation(unsigned long& calculations_per_frame)
{
    tree->add_bodies(bodies);
    tree->update(bodies, theta, G, dt, calculations_per_frame);
}

sf::Color HSLtoRGB(float hue, float saturation, float lightness)
{
    float chroma = (1 - std::abs(2 * lightness - 1)) * saturation;
    float huePrime = hue / 60.0f;
    float x = chroma * (1 - std::abs(std::fmod(huePrime, 2) - 1));

    float r, g, b;
    if (huePrime < 1.0f)
    {
        r = chroma;
        g = x;
        b = 0;
    }
    else if (huePrime < 2.0f)
    {
        r = x;
        g = chroma;
        b = 0;
    }
    else if (huePrime < 3.0f)
    {
        r = 0;
        g = chroma;
        b = x;
    }
    else if (huePrime < 4.0f)
    {
        r = 0;
        g = x;
        b = chroma;
    }
    else if (huePrime < 5.0f)
    {
        r = x;
        g = 0;
        b = chroma;
    }
    else
    {
        r = chroma;
        g = 0;
        b = x;
    }

    float m = lightness - chroma * 0.5f;
    float rgbOffset = m + chroma;

    return sf::Color(static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255));
}

void SimulationManager::draw_simulation()
{
    window->clear();

    // draw the quadtree
    if (draw_quadtree)
    {
        bounding_boxes.clear();
        tree->get_bounding_rectangles(bounding_boxes);

        for (sf::RectangleShape* rectangle : bounding_boxes)
        {
            window->draw(*rectangle);
        }
    }

    // draw the vectors
    if (draw_vectors)
    {
        for (Body* body : bodies)
        {
            double angle = atan2(body->vel.y, body->vel.x);
            double length = body->vel.length();

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(body->pos.x, body->pos.y)),
                sf::Vertex(sf::Vector2f(body->pos.x + cos(angle) * length, body->pos.y + sin(angle) * length))
            };

            line[0].color.a *= 0.3;
            line[1].color.a *= 0.3;

            window->draw(line, 2, sf::Lines);
        }
    }

    // find the min and max distance, gets abused for pressure
    double min_distance = std::numeric_limits<double>::max();
    double max_distance = std::numeric_limits<double>::min();
    for (Body* body : bodies)
    {
        double distance = body->pressure;
        min_distance = std::min(min_distance, distance);
        if (distance != std::numeric_limits<double>::max()) max_distance = std::max(max_distance, distance);
    }

    // draw bodies
    for (Body* body : bodies)
    {
        sf::CircleShape circle;
        circle.setRadius(body->radius);
        circle.setPosition(body->pos.x - body->radius, body->pos.y - body->radius);

        // color depends on distance to nearest body -> "pressure"
        double normalized_pressure = pow((body->pressure - min_distance) / (max_distance - min_distance), 0.5);
        sf::Color interpolatedColor;

        if (normalized_pressure <= 0.05)
        {
            interpolatedColor = sf::Color(255, 255, 255);
        }
        else if (normalized_pressure <= 0.5)
        {
            float hue = 240.0f * (0.5f - normalized_pressure) / 0.5f;  // Interpolate hue from 240 (blue) to 0 (red)
            interpolatedColor = HSLtoRGB(hue, 100, 50);
        }
        else
        {
            float hue = 0.0f;  // Red
            float saturation = 100.0f * (normalized_pressure - 0.5f) / 0.5f;  // Interpolate saturation from 0 to 100
            interpolatedColor = HSLtoRGB(hue, saturation, 100);
        }

        interpolatedColor.a = static_cast<sf::Uint8>(interpolatedColor.a * (0.6 + (0.4 * normalized_pressure)));

        circle.setFillColor(interpolatedColor);

        window->draw(circle);

        body->reset_pressure();
    }

    window->display();
    tree->clear();
}

void SimulationManager::handle_window_events()
{
    window->handle_events(toggle_paused, draw_quadtree, draw_vectors, debug);
}

void SimulationManager::print_debug_info(unsigned long steps, double elapsed_time, int calculations_per_frame, double worst_case, double best_case)
{
    double current_ratio_worst = worst_case / calculations_per_frame;
    double current_ratio_best = best_case / calculations_per_frame;

    std::cout << "########################################" << std::endl;
    std::cout << std::left << std::setw(20) << "particles: " << bodies.size() << std::endl;
    std::cout << std::left << std::setw(20) << "elapsed time: " << elapsed_time << " ms" << std::endl;
    std::cout << std::left << std::setw(20) << "STEP: " << steps << std::endl;
    std::cout << std::left << std::setw(20) << "fps: " << std::fixed << std::setprecision(3) << 1e3 * 1.0 / elapsed_time << std::endl;
    std::cout << std::left << std::setw(20) << "calc per frame: " << calculations_per_frame << std::endl;
    std::cout << std::left << std::setw(20) << "total calc: " << total_calculations << std::endl;
    std::cout << std::left << std::setw(20) << "worst case: " << std::fixed << std::setprecision(2) << current_ratio_worst * 100.0 << "% " << ((current_ratio_worst > 1.0) ? "faster" : "slower") << std::endl;
    std::cout << std::left << std::setw(20) << "best case: " << std::fixed << std::setprecision(2) << current_ratio_best * 100.0 << "% " << ((current_ratio_best > 1.0) ? "faster" : "slower") << std::endl;
    std::cout << "########################################" << std::endl << std::endl;
}
