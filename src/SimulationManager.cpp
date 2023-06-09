#include "SimulationManager.h"

SimulationManager::SimulationManager(const int width, const int height, const char* title, double G, double theta, double dt)
    : G(G), theta(theta), dt(dt), toggle_paused(true), toggle_draw_quadtree(false), toggle_draw_vectors(false), toggle_debug(false), total_calculations(0)
{
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = static_cast<double>(width);
    double ymax = static_cast<double>(height);

    bodies = std::make_shared<Bodies>(1000);
    tree = std::make_shared<QuadTree>(bodies, xmin, ymin, xmax, ymax);

    window = new Window(width, height, title);
    steps = 0;
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

        // bodies should rotate around the center of the screen, velocity is proportional to the distance to the center
        Vec2 center(window->get_width() / 2.0, window->get_height() / 2.0);
        Vec2 direction = (bodies->pos[i] - center).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(center) * 0.03;
    }

    if (toggle_verbose)
    {
        std::cout << "==> successfully added " << count << " bodies" << std::endl;
        // bodies->print();
    }
}

// ------------------------------------------------------------------------
// ====================================
// ========= SIMULATION LOOP ==========
// ====================================

void SimulationManager::print_start_info()
{
    std::cout << std::endl << std::endl;
    std::cout << "========== N-Body Simulation ===========" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "========= PRESS SPACE TO START =========" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "| Number of bodies: " << bodies->get_size() << std::endl;
    std::cout << "| G: " << G << std::endl;
    std::cout << "| theta: " << theta << std::endl;
    std::cout << "| dt: " << dt << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "| 'space' | 'q'  | 'v'  | 'd'   | 'esc' |" << std::endl;
    std::cout << "|  pause  | quad | vect | debug | exit  |" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "|  'left' |  'right' |  'up'  |  'down' |" << std::endl;
    std::cout << "|   dt -  |   dt +   |   G -  |   G +   |" << std::endl;
    std::cout << "========================================" << std::endl;
}

void SimulationManager::run()
{
    if (toggle_verbose) std::cout << "=> SimulationManager::run()" << std::endl;

    print_start_info();
    update_simulation(calculations_per_frame);

    double worst_case = bodies->get_size() * bodies->get_size();
    double best_case = bodies->get_size() * log2(bodies->get_size());

    std::chrono::steady_clock::time_point begin, end;

    while (window->is_open())
    {

        ++steps;
        handle_window_events();

        if (!toggle_paused)
        {
            begin = std::chrono::steady_clock::now();
            update_simulation(calculations_per_frame);
            end = std::chrono::steady_clock::now();

            elapsed_time_physics = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

            total_calculations += calculations_per_frame;

            if (toggle_debug)
            {
                print_debug_info();
            }
            calculations_per_frame = 0;
        }

        begin = std::chrono::steady_clock::now();
        draw_simulation();
        end = std::chrono::steady_clock::now();

        elapsed_time_graphics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
        total_frame_time = elapsed_time_physics + elapsed_time_graphics;
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

void SimulationManager::draw_simulation()
{
    window->clear();

    draw_quadtree();
    draw_vectors();

    draw_bodies();

    window->display();
}
// ------------------------------------------------------------------------
// ====================================
// ========= DRAW FUNCTIONS ===========
// ====================================

void SimulationManager::draw_vectors()
{
    if (!toggle_draw_vectors) return;

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

void SimulationManager::draw_quadtree()
{
    if (!toggle_draw_quadtree) return;

    bounding_boxes.clear();
    tree->get_bounding_rectangles(bounding_boxes);

    for (sf::RectangleShape* rectangle : bounding_boxes)
    {
        window->draw(*rectangle);
    }
}

sf::Color HSLtoRGB(float hue, float saturation, float lightness)
{
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

void SimulationManager::draw_bodies()
{
    unsigned max_density = 0;
    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        max_density = std::max(max_density, bodies->density[i]);
    }

    sf::CircleShape circle;
    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        double radius = 1.0;
        circle.setRadius(radius);
        circle.setPosition(bodies->pos[i].x - radius, bodies->pos[i].y - radius);

        // Normalize density to the range [0, 1]
        double normalized_density = static_cast<double>(bodies->density[i]) / max_density;

        // Apply sigmoid-shaped normalization
        double sigmoid_factor = 20.0;  // Adjust this value for the desired shape
        double sigmoid_normalized_density = 1.0 / (1.0 + std::exp(-sigmoid_factor * (normalized_density - 0.5)));

        // Map the normalized density to the color range
        sf::Color fill_color;
        if (normalized_density <= 0.5)
        {
            // Brown/Blueish to Red/Yellow transition
            unsigned blue_component = static_cast<unsigned>(255.0 * sigmoid_normalized_density * sigmoid_normalized_density);
            unsigned red_component = static_cast<unsigned>(255.0 * sigmoid_normalized_density);
            fill_color = sf::Color(red_component, blue_component, blue_component / 2);
        }
        else
        {
            // Red/Yellow to White transition
            unsigned red_component = static_cast<unsigned>(255.0 * sigmoid_normalized_density);
            unsigned green_component = static_cast<unsigned>(255.0 * sigmoid_normalized_density * sigmoid_normalized_density);
            fill_color = sf::Color(red_component, green_component, 0);
        }

        // Set the transparency based on density
        sf::Uint8 transparency = static_cast<sf::Uint8>(51u * bodies->density[i] / max_density);
        fill_color.a = (transparency * 5 < 150) ? 150 : transparency * 5;
        circle.setFillColor(fill_color);

        window->draw(circle);
    }
}

// ------------------------------------------------------------------------
// ====================================
// ========= EVENT HANDLING ===========
// ====================================

void SimulationManager::handle_window_events()
{
    if (toggle_verbose) std::cout << "=> SimulationManager::handle_window_events()" << std::endl;
    window->handle_events(toggle_paused, toggle_draw_quadtree, toggle_draw_vectors, toggle_debug, dt, G);
    if (toggle_verbose) std::cout << "==> successfully handled window events" << std::endl;
}

void SimulationManager::print_debug_info()
{
    std::cout << get_debug_info();
}

std::string SimulationManager::get_debug_info()
{
    std::stringstream ss;

    double current_ratio_worst = bodies->get_size() * bodies->get_size() / this->calculations_per_frame;
    double current_ratio_best = bodies->get_size() * log2(bodies->get_size()) / this->calculations_per_frame;

    ss << "########################################" << std::endl << std::endl;
    ss << std::left << std::setw(20) << "STEP: " << this->steps << std::endl;
    ss << std::left << std::setw(20) << "particles: " << this->bodies->get_size() << std::endl << std::endl;

    ss << std::left << std::setw(20) << "phys time: " << this->elapsed_time_physics / 1000 << " ms" << std::endl;
    ss << std::left << std::setw(20) << "graph time: " << this->elapsed_time_graphics / 1000 << " ms" << std::endl;
    ss << std::left << std::setw(20) << "total time: " << this->total_frame_time / 1000 << " ms" << std::endl << std::endl;

    ss << std::left << std::setw(20) << "fps: " << std::fixed << std::setprecision(3) << 1e6 * 1.0 / this->total_frame_time << std::endl << std::endl;

    ss << std::left << std::setw(20) << "calc per frame: " << this->calculations_per_frame << std::endl;
    ss << std::left << std::setw(20) << "total calc: " << this->total_calculations << std::endl << std::endl;

    //ss << std::left << std::setw(20) << "worst case: " << std::fixed << std::setprecision(2) << current_ratio_worst * 100.0 << "% " << ((current_ratio_worst > 1.0) ? "faster" : "slower") << std::endl;
    //ss << std::left << std::setw(20) << "best case: " << std::fixed << std::setprecision(2) << current_ratio_best * 100.0 << "% " << ((current_ratio_best > 1.0) ? "faster" : "slower") << std::endl << std::endl;

    return ss.str();
}
