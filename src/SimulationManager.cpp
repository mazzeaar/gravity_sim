#include "SimulationManager.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

SimulationManager::SimulationManager(const int width, const int height, const char* title, double G, double theta, double dt)
    : G(G), theta(theta), dt(dt), paused(true), draw_quadtree(false), draw_vectors(false), debug(false), total_calculations(0)
{
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = static_cast<double>(width);
    double ymax = static_cast<double>(height);

    bodies = std::make_shared<Bodies>(1000);
    bodies->set_size(width, height);

    std::shared_ptr<sf::VertexArray> rectangles = std::make_shared<sf::VertexArray>(sf::Lines, 0);
    tree = std::make_shared<QuadTree>(bodies, xmin, ymin, xmax, ymax, rectangles, true);

    particle_manager = std::make_shared<ParticleManager>(bodies, width, height);

    window = nullptr;
    steps = 0;
}

SimulationManager::~SimulationManager()
{
    bodies = nullptr;
    tree = nullptr;
    particle_manager = nullptr;
    window = nullptr;
}

/*----------------------------------------
|             public methods             |
-----------------------------------------*/

void SimulationManager::run()
{
    print_start_info();

    Vec2 top_left, bottom_right;
    particle_manager->get_particle_area(top_left, bottom_right);

    std::shared_ptr<sf::VertexArray> rectangles = std::make_shared<sf::VertexArray>(sf::Lines, 0);
    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right, rectangles, true);

    std::chrono::high_resolution_clock::time_point start_time;
    while ( window->is_open() )
    {
        this->calculations_per_frame = 0;
        elapsed_time_physics = 0;

        if ( !paused )
        {
            ++steps;

            start_time = std::chrono::high_resolution_clock::now();
            update_simulation();

            elapsed_time_physics = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start_time).count();
            this->total_calculations += calculations_per_frame;
        }

        start_time = std::chrono::high_resolution_clock::now();
        window->update();

        elapsed_time_graphics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
        total_frame_time = elapsed_time_physics + elapsed_time_graphics;
    }
}

void SimulationManager::add_bodies(unsigned count, double mass, BodyType body_type)
{
    particle_manager->add_bodies(body_type, count, mass);
}

void SimulationManager::update_simulation()
{
    Vec2 top_left, bottom_right; // the square that contains all particles
    particle_manager->get_particle_area(top_left, bottom_right);

    std::shared_ptr<sf::VertexArray> rectangles = std::make_shared<sf::VertexArray>(sf::Lines, 0);
    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right, rectangles, true);

    tree->update(theta, G, dt, calculations_per_frame);
}

// ====================================
// ========= EVENT HANDLING ===========
// ====================================

void SimulationManager::print_debug_info()
{
    std::cout << get_debug_info();
}

void SimulationManager::print_start_info()
{
    std::cout << get_start_info();
}

std::string SimulationManager::get_debug_info()
{
    std::stringstream ss;

    double current_ratio_worst = bodies->get_size() * bodies->get_size() / this->calculations_per_frame;
    double current_ratio_best = bodies->get_size() * log2(bodies->get_size()) / this->calculations_per_frame;

    ss << std::left << "|--- STEP: " << this->steps << std::endl;

    ss << std::left << "| particles: " << this->bodies->get_size() << std::endl;
    ss << std::left << "|--" << std::endl;

    ss << std::left << "| physics time: " << this->elapsed_time_physics / 1000 << " ms" << std::endl;
    ss << std::left << "| drawing time: " << this->elapsed_time_graphics / 1000 << " ms" << std::endl;
    ss << std::left << "| frame time: " << this->total_frame_time / 1000 << " ms" << std::endl;
    ss << std::left << "|--" << std::endl;

    ss << std::left << "| fps: " << std::fixed << std::setprecision(3) << 1e6 * 1.0 / this->total_frame_time << std::endl;
    ss << std::left << "|--" << std::endl;

    ss << std::left << "| interactions per frame: " << std::scientific << this->calculations_per_frame << std::endl;
    ss << std::left << "| total interactions: " << this->total_calculations << std::endl << std::endl;

    return ss.str();
}

std::string SimulationManager::get_start_info()
{
    std::stringstream ss;

    ss << "|           N-Body Simulation           |" << std::endl << std::endl;


    ss << "|  Number of bodies:     " << bodies->get_size() << std::endl;
    ss << "|  G:                                     " << std::scientific << std::setprecision(4) << G << std::endl;
    ss << "|  Theta:                             " << std::scientific << theta << std::endl;
    ss << "|  dt:                                    " << dt << std::endl << std::endl;

    ss << "|  'space'  | 'q'       | 'v'  | 'd'   | 'esc' |" << std::endl;
    ss << "|  pause    | quad | vect | debug | exit  |" << std::endl << std::endl;

    ss << "|  'left'  | 'right' |  'up'  | 'down' |" << std::endl;
    ss << "|  dt -    |  dt +   |   G -  |   G +  |" << std::endl;


    ss << "|          Press SPACE to start         |" << std::endl << std::endl;

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