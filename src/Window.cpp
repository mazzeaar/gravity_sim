#include "Window.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

Window::Window(int width, int height, const char* title, std::shared_ptr<SimulationManager> simulation_manager)
    : width(width), height(height)
{
    this->simulation_manager = simulation_manager;
    this->bodies = this->simulation_manager->get_bodies();

    font = sf::Font();
    font.loadFromFile("assets/fonts/montserrat/Montserrat-Regular.otf");

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

    if ( simulation_manager->get_toggle_paused() && simulation_manager->get_step() == 0 )
    {
        draw_start_screen();
        window->display();
        return;
    }

    draw_bodies();

    if ( simulation_manager->get_toggle_draw_vectors() )
    {
        draw_velocity_vectors();
    }

    if ( simulation_manager->get_toggle_draw_quadtree() )
    {
        draw_quadtree_bounds();
    }

    if ( simulation_manager->get_toggle_debug() )
    {
        draw_debug();
    }

    window->display();
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


/*----------------------------------------
|            private methods             |
-----------------------------------------*/

/*--------------------
|    draw methods    |
---------------------*/

void Window::draw_bodies()
{
    sf::Color low_density_color = sf::Color::Red;
    sf::Color high_density_color = sf::Color::White;

    auto interpolateColor = [&](double t) -> sf::Color // t is in range [0, 1]
    {
        sf::Uint8 r = static_cast<sf::Uint8>(low_density_color.r * (1.0 - t) + high_density_color.r * t);
        sf::Uint8 g = static_cast<sf::Uint8>(low_density_color.g * (1.0 - t) + high_density_color.g * t);
        sf::Uint8 b = static_cast<sf::Uint8>(low_density_color.b * (1.0 - t) + high_density_color.b * t);

        return sf::Color(r, g, b);
    };

    sf::VertexArray vertices(sf::Points);

    double highest_density = bodies->get_highest_density();
    double lowest_density = bodies->get_lowest_density();

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double normalized_density = (bodies->acc[i].length() - lowest_density) / (highest_density - lowest_density);

        sf::Color color = interpolateColor(normalized_density);

        sf::Vertex vertex(sf::Vector2f(bodies->pos[i].x, bodies->pos[i].y), color);

        vertices.append(vertex);
    }

    window->draw(vertices);
}

void Window::draw_velocity_vectors()
{
    const double lineLengthMultiplier = 0.4;

    sf::VertexArray lines(sf::Lines);
    lines.resize(bodies->get_size() * 2);

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double angle = atan2(bodies->vel[i].y, bodies->vel[i].x);
        double length = bodies->vel[i].length() * lineLengthMultiplier;

        sf::Vector2f startPos(bodies->pos[i].x, bodies->pos[i].y);
        sf::Vector2f endPos(bodies->pos[i].x + cos(angle) * length, bodies->pos[i].y + sin(angle) * length);

        sf::Vertex* line = &lines[i * 2];
        line[0].position = startPos;
        line[1].position = endPos;
        line[0].color.a = 76; // 0.3 * 255
        line[1].color.a = 76; // 0.3 * 255
    }

    window->draw(lines);
}


void Window::draw_quadtree_bounds()
{
    window->draw(*simulation_manager->get_bounding_rectangles());
}

void Window::draw_debug()
{
    sf::Text text;
    text.setPosition(10, 10);
    text.setFillColor(sf::Color::White);
    text.setFont(font);

    std::string debug_string = simulation_manager->get_debug_info();
    text.setString(debug_string);

    window->draw(text);
}

void Window::draw_start_screen()
{
    sf::Text text;

    text.setCharacterSize(50);
    text.setFillColor(sf::Color::White);
    text.setFont(font);

    std::string debug_string = simulation_manager->get_start_info();
    text.setString(debug_string);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

    window->draw(text);
}


/*--------------------
|    event handling   |
---------------------*/

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
|              old methods               |
-----------------------------------------*/

void Window::store_png(const std::string& filename)
{
    sf::Texture texture;
    texture.create(this->width, this->height);
    texture.update(*window);
    texture.copyToImage().saveToFile(filename);
}
