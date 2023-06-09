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
    if ( toggle_verbose ) std::cout << "=> SimulationManager::~SimulationManager()" << std::endl;

    bodies = nullptr;
    tree = nullptr;
    delete window;

    if ( toggle_verbose ) std::cout << "==> successfully deleted window and tree" << std::endl;
}

void SimulationManager::add_bodies(unsigned count, int max_mass)
{
    if ( toggle_verbose )
    {
        std::cout << "=> SimulationManager::add_bodies(" << count << ", " << max_mass << ")" << std::endl;
    }

    if ( count > bodies->get_size() )
    {
        bodies->resize(count);
    }

    /* square
    for ( unsigned i = 0; i < count; ++i )
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
    */

    /* spiral
     double center_x = window->get_width() / 2.0;
     double center_y = window->get_height() / 2.0;
     double max_distance = std::min(center_x, center_y) * 0.9; // Limit the maximum distance from the center

     double armCount = 5.0;       // Number of spiral arms
     double armTightness = 0.5;   // Tightness of the spiral arms
     double armVelocity = 0.03;   // Orbital velocity of the arms

     for ( unsigned i = 0; i < count; ++i )
     {
         bodies->mass[i] = static_cast<double>(rand() % max_mass + 1);
         bodies->radius[i] = std::pow(bodies->mass[i], 1.0 / 3.0);

         double angle = static_cast<double>(rand()) / RAND_MAX * 2.0 * M_PI; // Random angle in radians
         double distance = static_cast<double>(rand()) / RAND_MAX * max_distance; // Random distance from the center

         double totalArmAngle = 0.0;

         for ( int j = 0; j < armCount; ++j )
         {
             // Add spiral arm effect to the angle
             double armAngle = armTightness * angle + (j + 1) * distance / max_distance * 2.0 * M_PI;
             double x = center_x + distance * cos(armAngle + totalArmAngle);
             double y = center_y + distance * sin(armAngle + totalArmAngle);

             bodies->pos[i] = Vec2(x, y);

             Vec2 direction = (bodies->pos[i] - Vec2(center_x, center_y)).normalize();
             Vec2 perpendicular = Vec2(-direction.y, direction.x);

             bodies->vel[i] = perpendicular * distance * armVelocity;

             totalArmAngle += armAngle;
         }
     }
     */

    double center_x = window->get_width() / 2.0;
    double center_y = window->get_height() / 2.0;

    double center_top_left_x = window->get_width() / 4.0;
    double center_top_left_y = window->get_height() / 4.0;

    double center_bottom_right_x = window->get_width() * 3.0 / 4.0;
    double center_bottom_right_y = window->get_height() * 3.0 / 4.0;

    double cubeSize = 800.0; // Size of the cubes
    double speed = 0.04;     // Orbital speed of the cubes
    double mass = 10.0;

    for ( unsigned i = 0; i < bodies->get_size() / 3; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;

        double x = center_top_left_x + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        double y = center_top_left_y + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        bodies->pos[i] = Vec2(x, y);

        Vec2 direction = (bodies->pos[i] - Vec2(center_top_left_x, center_top_left_y)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(Vec2(center_top_left_x, center_top_left_y)) * speed;
    }

    for ( unsigned i = bodies->get_size() / 3; i < 2 * bodies->get_size() / 3; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = cubeSize;

        double x = center_bottom_right_x + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        double y = center_bottom_right_y + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        bodies->pos[i] = Vec2(x, y);

        Vec2 direction = (bodies->pos[i] - Vec2(center_bottom_right_x, center_bottom_right_y)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(Vec2(center_bottom_right_x, center_bottom_right_y)) * speed;
    }

    // add the rest randomly
    for ( unsigned i = 2 * bodies->get_size() / 3; i < bodies->get_size(); ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;

        // position fills 4/5 of the screen
        double x = rand() % (4 * window->get_width() / 5) + window->get_width() / 10;
        double y = rand() % (4 * window->get_height() / 5) + window->get_height() / 10;

        bodies->pos[i] = Vec2(x, y);
    }

    if ( toggle_verbose )
    {
        std::cout << "==> successfully added " << count << " bodies" << std::endl;
        // bodies->print();
    }
}

void SimulationManager::get_particle_area(Vec2& top_left, Vec2& bottom_right)
{
    if ( toggle_verbose ) std::cout << "=> SimulationManager::get_particle_area()" << std::endl;
    // the area must be a square
    top_left = Vec2(window->get_width(), window->get_height());
    bottom_right = Vec2(0, 0);

    // find the bounding square that contains all particles
    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        if ( bodies->pos[i].x < top_left.x )
        {
            top_left.x = bodies->pos[i].x;
        }

        if ( bodies->pos[i].x > bottom_right.x )
        {
            bottom_right.x = bodies->pos[i].x;
        }

        if ( bodies->pos[i].y < top_left.y )
        {
            top_left.y = bodies->pos[i].y;
        }

        if ( bodies->pos[i].y > bottom_right.y )
        {
            bottom_right.y = bodies->pos[i].y;
        }
    }

    // fix the rectangle to be a square. make sure to center the particles
    double width = bottom_right.x - top_left.x;
    double height = bottom_right.y - top_left.y;

    if ( width > height )
    {
        top_left.y -= (width - height) / 2.0;
        bottom_right.y += (width - height) / 2.0;
    }
    else
    {
        top_left.x -= (height - width) / 2.0;
        bottom_right.x += (height - width) / 2.0;
    }

    if ( toggle_verbose ) std::cout << "==> successfully got particle area" << std::endl;
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
    if ( toggle_verbose ) std::cout << "=> SimulationManager::run()" << std::endl;

    Vec2 top_left, bottom_right;
    get_particle_area(top_left, bottom_right);

    tree = nullptr;
    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right);

    print_start_info();
    update_simulation(calculations_per_frame);

    double worst_case = bodies->get_size() * bodies->get_size();
    double best_case = bodies->get_size() * log2(bodies->get_size());

    std::chrono::steady_clock::time_point begin, end;

    while ( window->is_open() )
    {

        ++steps;
        handle_window_events();

        if ( !toggle_paused )
        {
            begin = std::chrono::steady_clock::now();
            update_simulation(calculations_per_frame);
            end = std::chrono::steady_clock::now();

            elapsed_time_physics = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

            total_calculations += calculations_per_frame;

            if ( toggle_debug )
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

// TODO - remove the print statements
void SimulationManager::update_simulation(unsigned long& calculations_per_frame)
{
    if ( toggle_verbose ) std::cout << std::endl << "=> SimulationManager::update_simulation()" << std::endl;

    Vec2 top_left, bottom_right;
    std::chrono::steady_clock::time_point begin, end;

    begin = std::chrono::steady_clock::now();

    get_particle_area(top_left, bottom_right);

    end = std::chrono::steady_clock::now();
    double get_particle_area_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    begin = std::chrono::steady_clock::now();

    tree = nullptr;
    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right);

    end = std::chrono::steady_clock::now();
    double make_shared_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    begin = std::chrono::steady_clock::now();

    tree->insert(bodies);

    end = std::chrono::steady_clock::now();
    double insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    begin = std::chrono::steady_clock::now();

    tree->update(theta, G, dt, calculations_per_frame);

    end = std::chrono::steady_clock::now();
    double update_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    if ( toggle_verbose )
    {
        std::cout << "get_particle_area: " << get_particle_area_time << " us" << std::endl;
        std::cout << "make_shared: " << make_shared_time << " us" << std::endl;
        std::cout << "insert: " << insert_time << " us" << std::endl;
        std::cout << "update: " << update_time << " us" << std::endl;
        std::cout << "total update time in ms: " << (get_particle_area_time + make_shared_time + insert_time + update_time) / 1000 << std::endl << std::endl;
        std::cout << "==> successfully updated simulation" << std::endl;
    }
}

void SimulationManager::draw_simulation()
{
    window->clear();

    draw_quadtree();
    draw_vectors();

    //draw_bodies_with_density();
    draw_bodies();

    window->display();
}
// ------------------------------------------------------------------------
// ====================================
// ========= DRAW FUNCTIONS ===========
// ====================================

void SimulationManager::draw_vectors()
{
    if ( !toggle_draw_vectors ) return;

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
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
    if ( !toggle_draw_quadtree ) return;

    bounding_boxes.clear();
    tree->get_bounding_rectangles(bounding_boxes);

    for ( sf::RectangleShape* rectangle : bounding_boxes )
    {
        window->draw(*rectangle);
    }
}

void SimulationManager::draw_bodies()
{
    sf::CircleShape circle;

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double radius = 1.0;
        circle.setRadius(radius);
        circle.setPosition(bodies->pos[i].x - radius, bodies->pos[i].y - radius);

        // set color to #F5F5DC
        circle.setFillColor(sf::Color{0x00F5F5DC});
        this->window->draw(circle);
    }
}

void SimulationManager::draw_bodies_with_density()
{
    unsigned max_density = 0;
    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        max_density = std::max(max_density, bodies->density[i]);
    }

    sf::CircleShape circle;
    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double radius = bodies->radius[i];
        circle.setRadius(radius);
        circle.setPosition(bodies->pos[i].x - radius, bodies->pos[i].y - radius);

        // Normalize density to the range [0, 1]
        double normalized_density = static_cast<double>(bodies->density[i]) / max_density;

        // Apply sigmoid-shaped normalization
        double sigmoid_factor = 20.0;  // Adjust this value for the desired shape
        double sigmoid_normalized_density = 1.0 / (1.0 + std::exp(-sigmoid_factor * (normalized_density - 0.5)));

        // Map the normalized density to the color range
        sf::Color fill_color;
        if ( normalized_density <= 0.5 )
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

sf::Color HSLtoRGB(float hue, float saturation, float lightness)
{
    float chroma = (1 - std::abs(2 * lightness - 1)) * saturation;
    float hue_prime = hue / 60.0;

    float x = chroma * (1 - std::abs(fmod(hue_prime, 2) - 1));

    float red = 0.0;
    float green = 0.0;

    if ( hue_prime >= 0 && hue_prime < 1 )
    {
        red = chroma;
        green = x;
    }
    else if ( hue_prime >= 1 && hue_prime < 2 )
    {
        red = x;
        green = chroma;
    }
    else if ( hue_prime >= 2 && hue_prime < 3 )
    {
        green = chroma;
        red = -x;
    }
    else if ( hue_prime >= 3 && hue_prime < 4 )
    {
        green = x;
        red = -chroma;
    }
    else if ( hue_prime >= 4 && hue_prime < 5 )
    {
        red = -chroma;
        green = -x;
    }
    else if ( hue_prime >= 5 && hue_prime < 6 )
    {
        red = -x;
        green = -chroma;
    }

    float m = lightness - chroma / 2.0;

    return sf::Color((red + m) * 255, (green + m) * 255, (m) * 255);
}

// ------------------------------------------------------------------------
// ====================================
// ========= EVENT HANDLING ===========
// ====================================

void SimulationManager::handle_window_events()
{
    if ( toggle_verbose ) std::cout << "=> SimulationManager::handle_window_events()" << std::endl;
    window->handle_events(toggle_paused, toggle_draw_quadtree, toggle_draw_vectors, toggle_debug, dt, G);
    if ( toggle_verbose ) std::cout << "==> successfully handled window events" << std::endl;
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
