#include "Window.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

Window::Window(int width, int height, const char* title, std::shared_ptr<SimulationManager> simulation_manager)
    : width(width), height(height)
{
    this->simulation_manager = simulation_manager;
    this->bodies = this->simulation_manager->get_bodies();

    std::cout << bodies->get_size() << std::endl;

    window = new sf::RenderWindow(sf::VideoMode(width, height), title);
    window->setVerticalSyncEnabled(true);
    window->setPosition(sf::Vector2i(0, 0));
    window->setFramerateLimit(0);

    window->display();

}

Window::~Window()
{
    delete window;
}

/*----------------------------------------
|             public methods             |
-----------------------------------------*/

void Window::update()
{
    handle_events();

    window->clear();
    draw_bodies();

    if ( simulation_manager->get_toggle_draw_vectors() )
    {
        draw_velocity_vectors();
    }

    if ( simulation_manager->get_toggle_draw_quadtree() )
    {
        draw_quadtree_bounds();
    }

    window->display();
}

/*----------------------------------------
|            private methods             |
-----------------------------------------*/

void Window::draw_bodies()
{
    sf::Color low_density_color = sf::Color::Red;
    sf::Color high_density_color = sf::Color::White;

    auto interpolateColor = [&](double t) -> sf::Color
    {
        // Interpolate the red, green, blue
        sf::Uint8 r = static_cast<sf::Uint8>(low_density_color.r * (1.0 - t) + high_density_color.r * t);
        sf::Uint8 g = static_cast<sf::Uint8>(low_density_color.g * (1.0 - t) + high_density_color.g * t);
        sf::Uint8 b = static_cast<sf::Uint8>(low_density_color.b * (1.0 - t) + high_density_color.b * t);

        // Return the interpolated color
        return sf::Color(r, g, b);
    };

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

        sf::Color color = interpolateColor(normalized_density);
        sf::Vertex vertex(sf::Vector2f(bodies->pos[i].x, bodies->pos[i].y), color);

        vertices.append(vertex);
    }

    window->draw(vertices);
}

void Window::draw_velocity_vectors()
{
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

void Window::draw_quadtree_bounds()
{
    window->draw(*simulation_manager->get_bounding_rectangles());
}

void Window::handle_events()
{
    sf::Event event;
    sf::RenderWindow& currentWindow = *window;  // Store a reference to the window

    while ( currentWindow.pollEvent(event) )
    {
        if ( event.type == sf::Event::Closed )
        {
            currentWindow.close();
        }
        else if ( event.type == sf::Event::KeyPressed )
        {
            if ( event.key.code == sf::Keyboard::Escape )
            {
                currentWindow.close();
                std::cout << "closing window" << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::Space )
            {
                simulation_manager->toggle_pause();
                std::cout << "toggle_pause = " << simulation_manager->get_toggle_paused() << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::Q )
            {
                simulation_manager->toggle_draw_quadtree();
                std::cout << "toggle_draw_quadtree = " << simulation_manager->get_toggle_draw_quadtree() << std::endl;
            }

            else if ( event.key.code == sf::Keyboard::V )
            {
                simulation_manager->toggle_draw_vectors();
                std::cout << "toggle_draw_vectors = " << simulation_manager->get_toggle_draw_vectors() << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::D )
            {
                simulation_manager->toggle_debug_info();
                std::cout << "toggle_debug = " << simulation_manager->get_toggle_debug() << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::Right )
            {
                simulation_manager->increase_dt();
                std::cout << "increased dt to " << simulation_manager->get_dt() << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::Left )
            {
                simulation_manager->decrease_dt();
                std::cout << "decreased dt to " << simulation_manager->get_dt() << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::Up )
            {
                simulation_manager->increase_G();
                std::cout << "increased G to " << simulation_manager->get_G() << std::endl;
            }
            else if ( event.key.code == sf::Keyboard::Down )
            {
                simulation_manager->decrease_G();
                std::cout << "decreased G to " << simulation_manager->get_G() << std::endl;
            }
        }
    }
}


/*----------------------------------------
|            old methods             |
-----------------------------------------*/


void Window::store_png(const std::string& filename)
{
    sf::Texture texture;
    texture.create(this->width, this->height);
    texture.update(*window);
    texture.copyToImage().saveToFile(filename);
}

int Window::get_width()
{
    return width;
}

int Window::get_height()
{
    return height;
}

bool Window::is_open()
{
    return window->isOpen();
}