#include "SimulationManager.h"

SimulationManager::SimulationManager(const int width, const int height, const char* title, double G, double theta, double dt)
    : G(G), theta(theta), dt(dt), toggle_paused(false), toggle_draw_quadtree(false), toggle_draw_vectors(false), toggle_debug(false), total_calculations(0)
{
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = static_cast<double>(width);
    double ymax = static_cast<double>(height);

    bodies = std::make_shared<Bodies>(1000);
    tree = std::make_shared<QuadTree>(bodies, xmin, ymin, xmax, ymax);

    window = new Window(width, height, title);
}

SimulationManager::~SimulationManager()
{
    if (toggle_verbose) std::cout << "=> SimulationManager::~SimulationManager()" << std::endl;

    bodies = nullptr;
    tree = nullptr;
    delete window;

    if (toggle_verbose) std::cout << "==> successfully deleted window and tree" << std::endl;
}

void SimulationManager::add_bodies(unsigned count, int max_mass)
{
    if (toggle_verbose)
    {
        std::cout << "=> SimulationManager::add_bodies(" << count << ", " << max_mass << ")" << std::endl;
    }

    if (count > bodies->get_size())
    {
        bodies->resize(count);
    }

    for (unsigned i = 0; i < count; ++i)
    {
        bodies->mass[i] = static_cast<double>(rand() % max_mass + 1);
        bodies->radius[i] = std::pow(bodies->mass[i], 1.0 / 3.0);

        double x = rand() % (3 * window->get_width() / 5) + window->get_width() / 5;
        double y = rand() % (3 * window->get_height() / 5) + window->get_height() / 5;

        bodies->pos[i] = Vec2(x, y);

        double angle = atan2(y - window->get_height() / 2.0, x - window->get_width() / 2.0);

        bodies->vel[i] = Vec2(-sin(angle), cos(angle)) * 10;

        //bodies->vel[i] = Vec2(0.0, 0.0);
    }

    if (toggle_verbose)
    {
        std::cout << "==> successfully added " << count << " bodies" << std::endl;
        // bodies->print();
    }
}

void SimulationManager::run()
{
    if (toggle_verbose) std::cout << "=> SimulationManager::run()" << std::endl;

    double worst_case = bodies->get_size() * bodies->get_size();
    double best_case = bodies->get_size() * log2(bodies->get_size());
    unsigned long steps = 0;

    while (window->is_open())
    {
        ++steps;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        handle_window_events();

        if (!toggle_paused)
        {
            update_simulation(calculations_per_frame);

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            total_calculations += calculations_per_frame;
            if (toggle_debug)
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
    if (toggle_verbose) std::cout << "=> SimulationManager::update_simulation()" << std::endl;

    tree = nullptr;
    tree = std::make_shared<QuadTree>(bodies, 0.0, 0.0, static_cast<double>(window->get_width()), static_cast<double>(window->get_height()));

    tree->insert(bodies);
    tree->update(theta, G, dt, calculations_per_frame);

    if (toggle_verbose) std::cout << "==> successfully updated simulation" << std::endl;
}

sf::Color HSLtoRGB(float hue, float saturation, float lightness)
{
    // no pressure = green // more pressure = red
    float chroma = (1 - std::abs(2 * lightness - 1)) * saturation;
    float hue_prime = hue / 60.0;

    float x = chroma * (1 - std::abs(fmod(hue_prime, 2) - 1));

    float red = 0.0;
    float green = 0.0;

    if (hue_prime >= 0 && hue_prime < 1)
    {
        red = chroma;
        green = x;
    }
    else if (hue_prime >= 1 && hue_prime < 2)
    {
        red = x;
        green = chroma;
    }
    else if (hue_prime >= 2 && hue_prime < 3)
    {
        green = chroma;
        red = -x;
    }
    else if (hue_prime >= 3 && hue_prime < 4)
    {
        green = x;
        red = -chroma;
    }
    else if (hue_prime >= 4 && hue_prime < 5)
    {
        red = -chroma;
        green = -x;
    }
    else if (hue_prime >= 5 && hue_prime < 6)
    {
        red = -x;
        green = -chroma;
    }

    float m = lightness - chroma / 2.0;

    return sf::Color((red + m) * 255, (green + m) * 255, (m) * 255);
}

void SimulationManager::draw_simulation()
{
    window->clear();

    // draw the quadtree
    if (toggle_draw_quadtree)
    {
        bounding_boxes.clear();
        tree->get_bounding_rectangles(bounding_boxes);

        for (sf::RectangleShape* rectangle : bounding_boxes)
        {
            window->draw(*rectangle);
        }
    }

    // draw the vectors
    if (toggle_draw_vectors)
    {
        for (unsigned i = 0; i < bodies->get_size(); ++i)
        {
            double angle = atan2(bodies->vel[i].y, bodies->vel[i].x);
            double length = bodies->vel[i].length() + 15;

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(bodies->pos[i].x, bodies->pos[i].y)),
                sf::Vertex(sf::Vector2f(bodies->pos[i].x + cos(angle) * length, bodies->pos[i].y + sin(angle) * length))
            };

            line[0].color.a *= 0.3;
            line[1].color.a *= 0.3;

            window->draw(line, 2, sf::Lines);
        }
    }

    // find the min and max distance, gets abused for pressure
    double min_distance = std::numeric_limits<double>::max();
    double max_distance = 0.0;

    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        double distance = bodies->pressure[i];
        min_distance = std::min(min_distance, distance);
        max_distance = std::max(max_distance, distance);
    }

    // draw bodies
    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        sf::CircleShape circle;
        double radius = bodies->radius[i];
        circle.setRadius(radius);
        circle.setPosition(sf::Vector2f(bodies->pos[i].x - radius, bodies->pos[i].y - radius));

        // interpolate between red and blue
        double hue = 240.0 * (bodies->pressure[i] - min_distance) / (max_distance - min_distance);
        sf::Color interpolatedColor = HSLtoRGB(hue, 1.0, 0.5);

        circle.setFillColor(interpolatedColor);

        window->draw(circle);

        bodies->reset_pressure(i);
    }

    window->display();
}

void SimulationManager::handle_window_events()
{
    if (toggle_verbose) std::cout << "=> SimulationManager::handle_window_events()" << std::endl;
    window->handle_events(toggle_paused, toggle_draw_quadtree, toggle_draw_vectors, toggle_debug, dt);
    if (toggle_verbose) std::cout << "==> successfully handled window events" << std::endl;
}

void SimulationManager::print_debug_info(unsigned long steps, double elapsed_time, int calculations_per_frame, double worst_case, double best_case)
{
    double current_ratio_worst = worst_case / calculations_per_frame;
    double current_ratio_best = best_case / calculations_per_frame;

    std::cout << "########################################" << std::endl;
    std::cout << std::left << std::setw(20) << "particles: " << bodies->get_size() << std::endl;
    std::cout << std::left << std::setw(20) << "elapsed time: " << elapsed_time << " ms" << std::endl;
    std::cout << std::left << std::setw(20) << "STEP: " << steps << std::endl;
    std::cout << std::left << std::setw(20) << "fps: " << std::fixed << std::setprecision(3) << 1e3 * 1.0 / elapsed_time << std::endl;
    std::cout << std::left << std::setw(20) << "calc per frame: " << calculations_per_frame << std::endl;
    std::cout << std::left << std::setw(20) << "total calc: " << total_calculations << std::endl;
    std::cout << std::left << std::setw(20) << "worst case: " << std::fixed << std::setprecision(2) << current_ratio_worst * 100.0 << "% " << ((current_ratio_worst > 1.0) ? "faster" : "slower") << std::endl;
    std::cout << std::left << std::setw(20) << "best case: " << std::fixed << std::setprecision(2) << current_ratio_best * 100.0 << "% " << ((current_ratio_best > 1.0) ? "faster" : "slower") << std::endl;
    std::cout << "########################################" << std::endl << std::endl;
}
