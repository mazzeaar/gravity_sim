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
    particle_manager = std::make_shared<ParticleManager>(bodies, width, height);

    window = new Window(width, height, title);
    steps = 0;
}

SimulationManager::~SimulationManager()
{
    bodies = nullptr;
    tree = nullptr;
    delete window;
}

void SimulationManager::add_bodies(unsigned count, double mass, BodyType body_type)
{
    particle_manager->add_bodies(body_type, count, mass);
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
    print_start_info();

    Vec2 top_left, bottom_right;
    particle_manager->get_particle_area(top_left, bottom_right);

    tree = nullptr;
    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right, true);

    update_simulation(calculations_per_frame);

    double worst_case = bodies->get_size() * bodies->get_size();
    double best_case = bodies->get_size() * log2(bodies->get_size());

    std::chrono::steady_clock::time_point begin;

    while ( window->is_open() )
    {
        ++steps;

        std::string filename = std::to_string(steps) + ".png";

        while ( filename.size() < 7 )
        {
            filename = "0" + filename;
        }
        //window->store_png("./images/dump/" + filename);

        handle_window_events();

        if ( !toggle_paused )
        {
            // update the forces and positions of the bodies
            begin = std::chrono::steady_clock::now();
            update_simulation(calculations_per_frame);
            elapsed_time_physics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();

            total_calculations += calculations_per_frame;

            if ( toggle_debug ) print_debug_info();
            calculations_per_frame = 0;
        }

        // draw the bodies
        begin = std::chrono::steady_clock::now();
        draw_simulation();

        elapsed_time_graphics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
        total_frame_time = elapsed_time_physics + elapsed_time_graphics;
    }
}

// TODO - remove the print statements
void SimulationManager::update_simulation(unsigned long& calculations_per_frame)
{
    Vec2 top_left, bottom_right; // the square that contains all particles
    particle_manager->get_particle_area(top_left, bottom_right);

    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right, true);

    tree->update(theta, G, dt, calculations_per_frame);
}

void SimulationManager::draw_simulation(bool half_step)
{
    window->clear();

    if ( !half_step )
    {
        draw_quadtree();
    }
    else
    {
        draw_old_quadtree();
    }

    draw_vectors();

    draw_bodies();
    window->display();
}


// ====================================
// ========= DRAW FUNCTIONS ===========
// ====================================

sf::Color interpolateColor(const sf::Color& color1, const sf::Color& color2, double t)
{
    // Interpolate the red, green, blue
    sf::Uint8 r = static_cast<sf::Uint8>(color1.r * (1.0 - t) + color2.r * t);
    sf::Uint8 g = static_cast<sf::Uint8>(color1.g * (1.0 - t) + color2.g * t);
    sf::Uint8 b = static_cast<sf::Uint8>(color1.b * (1.0 - t) + color2.b * t);

    // Return the interpolated color
    return sf::Color(r, g, b);
}

void SimulationManager::draw_bodies()
{
    sf::Color low_density_color = sf::Color::Red;
    sf::Color high_density_color = sf::Color::White;

    sf::VertexArray vertices(sf::Points);

    double highest_density = std::numeric_limits<double>::min();
    double lowest_density = std::numeric_limits<double>::max();

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        highest_density = std::max(highest_density, bodies->acc[i].length());
        lowest_density = std::min(lowest_density, bodies->acc[i].length());
    }

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double normalized_density = bodies->acc[i].length() / highest_density;

        sf::Color color = interpolateColor(low_density_color, high_density_color, normalized_density);
        sf::Vertex vertex(sf::Vector2f(bodies->pos[i].x, bodies->pos[i].y), color);

        vertices.append(vertex);
    }

    this->window->draw(vertices);
}


void SimulationManager::draw_vectors()
{
    if ( !toggle_draw_vectors ) return;

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double angle = atan2(bodies->vel[i].y, bodies->vel[i].x);
        double length = bodies->vel[i].length() + 10.0;

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(bodies->pos[i].x, bodies->pos[i].y)),
            sf::Vertex(sf::Vector2f(bodies->pos[i].x + cos(angle) * length, bodies->pos[i].y + sin(angle) * length))
        };

        line[0].color.a *= 0.3;
        line[1].color.a *= 0.3;

        window->draw(line, 2, sf::Lines);
    }
}

void SimulationManager::draw_old_quadtree()
{
    if ( !toggle_draw_quadtree ) return;

    for ( sf::RectangleShape* rectangle : bounding_boxes )
    {
        window->draw(*rectangle);
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

// ====================================
// ========= EVENT HANDLING ===========
// ====================================

void SimulationManager::handle_window_events()
{
    window->handle_events(toggle_paused, toggle_draw_quadtree, toggle_draw_vectors, toggle_debug, dt, G);
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


    ss << std::left << std::setw(21) << "|--- STEP: " << this->steps << std::endl;

    ss << std::left << std::setw(21) << "| particles: " << this->bodies->get_size() << std::endl;
    ss << std::left << "|" << std::endl;

    ss << std::left << std::setw(21) << "| phys time: " << this->elapsed_time_physics / 1000 << " ms" << std::endl;
    ss << std::left << std::setw(21) << "| graph time: " << this->elapsed_time_graphics / 1000 << " ms" << std::endl;
    ss << std::left << std::setw(21) << "| total time: " << this->total_frame_time / 1000 << " ms" << std::endl;
    ss << std::left << "|" << std::endl;

    ss << std::left << std::setw(21) << "| fps: " << std::fixed << std::setprecision(3) << 1e6 * 1.0 / this->total_frame_time << std::endl;
    ss << std::left << "|" << std::endl;

    ss << std::left << std::setw(21) << "| calc per frame: " << this->calculations_per_frame << std::endl;
    ss << std::left << std::setw(21) << "| total calc: " << this->total_calculations << std::endl << std::endl;

    return ss.str();
}








// ====================================
// functions to draw fourteen segment display letters
// ====================================

// clean this up in a different class. maybe make modular

/*
void horizontal_line(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    for ( unsigned i = index; i < index + amount; ++i )
    {
        double x_pos = rand() % width + x;
        double y_pos = y + (rand() % thickness - thickness / 2);  // Adjust y randomly within thickness

        bodies->pos[i] = Vec2(x_pos, y_pos);

        bodies->mass[i] = 10.0;
        bodies->radius[i] = 1.0;

        bodies->to_be_deleted[i] = false;
    }
}

void vertical_line(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    for ( unsigned i = index; i < index + amount; ++i )
    {
        double x_pos = x + (rand() % thickness - thickness / 2);  // Adjust x randomly within thickness
        double y_pos = rand() % height + y;

        bodies->pos[i] = Vec2(x_pos, y_pos);

        bodies->mass[i] = 10.0;
        bodies->radius[i] = 1.0;

        bodies->to_be_deleted[i] = false;
    }
}

void diagonal_line_left(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    for ( unsigned i = index; i < index + amount; ++i )
    {
        double ratio = (double) (i - index) / amount;
        double x_pos = ratio * width + x + (rand() % thickness - thickness / 2);
        double y_pos = ratio * height + y + (rand() % thickness - thickness / 2);

        bodies->pos[i] = Vec2(x_pos, y_pos);

        bodies->mass[i] = 10.0;
        bodies->radius[i] = 1.0;

        bodies->to_be_deleted[i] = false;
    }
}

void diagonal_line_right(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    for ( unsigned i = index; i < index + amount; ++i )
    {
        double ratio = (double) (i - index) / amount;
        double x_pos = (1 - ratio) * width + x + (rand() % thickness - thickness / 2);  // invert ratio for right diagonal
        double y_pos = ratio * height + y + (rand() % thickness - thickness / 2);

        bodies->pos[i] = Vec2(x_pos, y_pos);

        bodies->mass[i] = 10.0;
        bodies->radius[i] = 1.0;

        bodies->to_be_deleted[i] = false;
    }
}


// functions to draw parts of 14 segment display letters
void horizontal_top(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    horizontal_line(bodies, index, amount, x, y + thickness / 2, width, thickness, thickness);
}

void horizontal_bottom(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    horizontal_line(bodies, index, amount, x, y + height - thickness / 2, width, thickness, thickness);
}

void horizontal_middle_left(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    horizontal_line(bodies, index, amount, x, y + height / 2, width / 2, thickness, thickness);
}

void horizontal_middle_right(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    horizontal_line(bodies, index, amount, x + width / 2, y + height / 2, width / 2, thickness, thickness);
}

void vertical_top_left(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    vertical_line(bodies, index, amount, x + thickness / 2, y, thickness, height / 2, thickness);
}

void vertical_top_right(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    vertical_line(bodies, index, amount, x + width - thickness / 2, y, thickness, height / 2, thickness);
}

void vertical_bottom_left(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    vertical_line(bodies, index, amount, x + thickness / 2, y + height / 2, thickness, height / 2, thickness);
}

void vertical_bottom_right(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    vertical_line(bodies, index, amount, x + width - thickness / 2, y + height / 2, thickness, height / 2, thickness);
}

void vertical_top_middle(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    vertical_line(bodies, index, amount, x + width / 2, y, thickness, height / 2, thickness);
}

void vertical_bottom_middle(std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    vertical_line(bodies, index, amount, x + width / 2, y + height / 2, thickness, height / 2, thickness);
}

void draw_letter(std::vector<bool> segments, std::shared_ptr<Bodies> bodies, unsigned index, unsigned amount, unsigned x, unsigned y, unsigned width, unsigned height, unsigned thickness)
{
    // every segment has the same amount of particles, except the middle segment. the middle segment has half the amount of particles and is index 0 and 1
    unsigned segment_count = [segments]() -> unsigned
    {
        unsigned count = (segments[0] + segments[1]) / 2;
        for ( unsigned i = 2; i < segments.size(); ++i )
        {
            if ( segments[i] )
            {
                ++count;
            }
        }
        return count;
    }();

    if ( segments[0] )
    {
        horizontal_middle_left(bodies, index, (amount / segment_count) / 2, x, y, width, height, thickness);
    }

    if ( segments[1] )
    {
        horizontal_middle_right(bodies, index + (amount / segment_count) / 2, (amount / segment_count) / 2, x, y, width, height, thickness);
    }

    if ( segments[2] )
    {
        horizontal_top(bodies, index + amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[3] )
    {
        horizontal_bottom(bodies, index + 2 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[4] )
    {
        vertical_top_left(bodies, index + 3 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[5] )
    {
        vertical_top_right(bodies, index + 4 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[6] )
    {
        vertical_bottom_left(bodies, index + 5 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[7] )
    {
        vertical_bottom_right(bodies, index + 6 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[8] )
    {
        vertical_top_middle(bodies, index + 7 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[9] )
    {
        vertical_bottom_middle(bodies, index + 8 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[10] )
    {
        diagonal_line_left(bodies, index + 9 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[11] )
    {
        diagonal_line_right(bodies, index + 10 * amount / segment_count, amount / segment_count, x, y, width, height, thickness);
    }

    if ( segments[12] )
    {
        diagonal_line_left(bodies, index + 11 * amount / segment_count, amount / segment_count, x, y + height, width, -height, thickness);
    }

    if ( segments[13] )
    {
        diagonal_line_right(bodies, index + 12 * amount / segment_count, amount / segment_count, x, y + height, width, -height, thickness);
    }
}

void add_lisa(unsigned count, double mass)
{
    if ( count < 4 )
    {
        std::cout << "Error: Insufficient number of bodies to draw 'LISA'." << std::endl;
        return;
    }

    if ( count > bodies->get_size() )
    {
        bodies->resize(count);
    }

    // Calculate the size of each letter there should be nice spacing between the letters and the edges of the screen and the letters themselves
    unsigned letterWidth = window->get_width() / 2.0;
    unsigned letterHeight = window->get_height() / 6.0;

    unsigned spacing_top = letterHeight / 2.0;
    unsigned spacing_bottom = letterHeight / 2.0;
    unsigned spacing_left = letterWidth / 2.0;
    unsigned spacing_right = letterWidth / 2.0;

    unsigned spacing_between_letters = letterHeight / 3.0;

    // lambda that returns the position of the top left corner of the letter based on the letter index
    auto get_top_left = [=](unsigned index) -> Vec2
    {
        unsigned y = spacing_top + (index * (letterHeight + spacing_between_letters));
        unsigned x = spacing_left;

        return Vec2(x, y);
    };

    unsigned particles_per_letter = count / 4;
    unsigned thickness = 10;

    // L
    horizontal_bottom(bodies, 0, particles_per_letter / 3, get_top_left(0).x, get_top_left(0).y, letterWidth, letterHeight, thickness);
    vertical_top_left(bodies, particles_per_letter / 3, particles_per_letter / 3, get_top_left(0).x, get_top_left(0).y, letterWidth, letterHeight, thickness);
    vertical_bottom_left(bodies, 2 * particles_per_letter / 3, particles_per_letter / 3, get_top_left(0).x, get_top_left(0).y, letterWidth, letterHeight, thickness);

    // I
    vertical_top_middle(bodies, particles_per_letter, particles_per_letter / 2, get_top_left(1).x, get_top_left(1).y, letterWidth, letterHeight, thickness);
    vertical_bottom_middle(bodies, particles_per_letter + particles_per_letter / 2, particles_per_letter / 2, get_top_left(1).x, get_top_left(1).y, letterWidth, letterHeight, thickness);

    // S
    horizontal_top(bodies, 2 * particles_per_letter, particles_per_letter / 5, get_top_left(2).x, get_top_left(2).y, letterWidth, letterHeight, thickness);
    horizontal_bottom(bodies, 2 * particles_per_letter + particles_per_letter / 5, particles_per_letter / 5, get_top_left(2).x, get_top_left(2).y, letterWidth, letterHeight, thickness);

    vertical_top_left(bodies, 2 * particles_per_letter + 2 * particles_per_letter / 5, particles_per_letter / 5, get_top_left(2).x, get_top_left(2).y, letterWidth, letterHeight, thickness);
    vertical_bottom_right(bodies, 2 * particles_per_letter + 3 * particles_per_letter / 5, particles_per_letter / 5, get_top_left(2).x, get_top_left(2).y, letterWidth, letterHeight, thickness);

    horizontal_middle_left(bodies, 2 * particles_per_letter + 4 * particles_per_letter / 5, particles_per_letter / 10, get_top_left(2).x, get_top_left(2).y, letterWidth, letterHeight, thickness);
    horizontal_middle_right(bodies, 2 * particles_per_letter + 9 * particles_per_letter / 10, particles_per_letter / 10, get_top_left(2).x, get_top_left(2).y, letterWidth, letterHeight, thickness);

    // A
    horizontal_top(bodies, 3 * particles_per_letter, particles_per_letter / 6, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);

    vertical_bottom_right(bodies, 3 * particles_per_letter + particles_per_letter / 6, particles_per_letter / 5, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);
    vertical_bottom_left(bodies, 3 * particles_per_letter + 2 * particles_per_letter / 6, particles_per_letter / 5, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);

    vertical_top_left(bodies, 3 * particles_per_letter + 3 * particles_per_letter / 6, particles_per_letter / 5, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);
    vertical_top_right(bodies, 3 * particles_per_letter + 4 * particles_per_letter / 6, particles_per_letter / 5, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);

    horizontal_middle_left(bodies, 3 * particles_per_letter + 5 * particles_per_letter / 6, particles_per_letter / 12, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);
    horizontal_middle_right(bodies, 3 * particles_per_letter + 11 * particles_per_letter / 12, particles_per_letter / 12, get_top_left(3).x, get_top_left(3).y, letterWidth, letterHeight, thickness);

    bodies->remove_merged_bodies();

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;
    }
}
*/