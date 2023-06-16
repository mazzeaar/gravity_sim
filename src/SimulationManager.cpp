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

            elapsed_time_physics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
            this->total_calculations += calculations_per_frame;
        }

        start_time = std::chrono::high_resolution_clock::now();
        window->update();

        elapsed_time_graphics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
        total_frame_time = elapsed_time_physics + elapsed_time_graphics;

        // THIS IS SHIT, BUT IF I DONT DO IT LIKE THAT THE QUADTREE IS ALWAYS OFF BY ONE FRAME:)
        if ( !paused ) bodies->update(dt);
    }
}

/*
tried multithreading, sadly this code is slower than the single threaded version
it also had some issues with stats (var was not atomic), but too lazy to fix it
void SimulationManager::run()
{
    std::chrono::high_resolution_clock::time_point start_time;

    Vec2 top_left, bottom_right;
    particle_manager->get_particle_area(top_left, bottom_right);

    std::atomic<bool> stopSimulation(false);

    // sim update thread
    std::thread simulationThread([&]() {
        while (!stopSimulation) {
            calculations_per_frame = 0;
            elapsed_time_physics = 0;

            if (!paused) {
                ++steps;

                start_time = std::chrono::high_resolution_clock::now();
                update_simulation();

                elapsed_time_physics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
                total_calculations += calculations_per_frame;
            }
        }
    });

    // window update in main thread
    while (window->is_open()) {
        start_time = std::chrono::high_resolution_clock::now();
        window->update();
        elapsed_time_graphics = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
        total_frame_time = elapsed_time_physics + elapsed_time_graphics;
    }

    stopSimulation = true;
    simulationThread.join();
}
*/


void SimulationManager::update_simulation()
{
    Vec2 top_left, bottom_right;
    particle_manager->get_particle_area(top_left, bottom_right);

    std::shared_ptr<sf::VertexArray> rectangles = std::make_shared<sf::VertexArray>(sf::Lines, 0);
    tree = std::make_shared<QuadTree>(bodies, top_left, bottom_right, rectangles, true);
    tree->update(theta, G, dt, calculations_per_frame);
}

double SimulationManager::get_current_ratio_worst_case()
{
    if ( calc_best_case == 0 || calc_worst_case == 0 )
    {
        calc_best_case = bodies->get_size() * bodies->get_size();
        calc_worst_case = bodies->get_size() * log2(bodies->get_size());
    }

    return calc_worst_case / static_cast<double>(this->calculations_per_frame);
}

double SimulationManager::get_current_ratio_best_case()
{
    if ( calc_best_case == 0 || calc_worst_case == 0 )
    {
        calc_best_case = bodies->get_size() * bodies->get_size();
        calc_worst_case = bodies->get_size() * log2(bodies->get_size());
    }

    return calc_best_case / static_cast<double>(this->calculations_per_frame);
}

double SimulationManager::get_average_ratio_best_case()
{
    if ( paused )
    {
        return average_ratio_best_case;
    }

    average_ratio_best_case = (average_ratio_best_case * (steps - 1) + get_current_ratio_best_case()) / steps;
    return average_ratio_best_case;
}

double SimulationManager::get_average_ratio_worst_case()
{
    if ( paused )
    {
        return average_ratio_worst_case;
    }

    average_ratio_worst_case = (average_ratio_worst_case * (steps - 1) + get_current_ratio_worst_case()) / steps;
    return average_ratio_worst_case;
}
