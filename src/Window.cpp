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
    double interpolation_cutoff = 0.2;

    sf::Color low_density_color = sf::Color(0, 128, 255);      // Light blue
    sf::Color mid_density_color = sf::Color(255, 0, 255);     // Magenta
    sf::Color high_density_color = sf::Color(255, 255, 0);    // Yellow

    auto interpolateColor = [&](double t) -> sf::Color // t is in range [0, 1]
    {
        if ( t < interpolation_cutoff )
        {
            double t2 = t / interpolation_cutoff;
            sf::Uint8 r = static_cast<sf::Uint8>(low_density_color.r * (1.0 - t2) + mid_density_color.r * t2);
            sf::Uint8 g = static_cast<sf::Uint8>(low_density_color.g * (1.0 - t2) + mid_density_color.g * t2);
            sf::Uint8 b = static_cast<sf::Uint8>(low_density_color.b * (1.0 - t2) + mid_density_color.b * t2);

            return sf::Color(r, g, b, 255);
        }
        else
        {
            double t2 = (t - interpolation_cutoff) / (1 - interpolation_cutoff);
            sf::Uint8 r = static_cast<sf::Uint8>(mid_density_color.r * (1.0 - t2) + high_density_color.r * t2);
            sf::Uint8 g = static_cast<sf::Uint8>(mid_density_color.g * (1.0 - t2) + high_density_color.g * t2);
            sf::Uint8 b = static_cast<sf::Uint8>(mid_density_color.b * (1.0 - t2) + high_density_color.b * t2);

            return sf::Color(r, g, b, 255);
        }
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
    const double lineLengthMultiplier = 1.0;

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
    unsigned left_offset = 20;
    unsigned top_offset = 20;
    unsigned text_top_offset = 50;
    unsigned values_left_offset = 230;

    sf::Text statusText;
    statusText.setPosition(left_offset, top_offset);
    statusText.setFont(font);
    statusText.setOutlineColor(sf::Color::Black);

    if ( simulation_manager->get_toggle_paused() )
    {
        statusText.setString("PAUSED");
        statusText.setFillColor(sf::Color::Red);
    }
    else
    {
        statusText.setString("RUNNING");
        statusText.setFillColor(sf::Color::Green);
    }

    window->draw(statusText);

    sf::Text names;
    names.setPosition(left_offset, top_offset + text_top_offset);
    names.setFont(font);
    names.setFillColor(sf::Color::White);
    names.setOutlineColor(sf::Color::Black);

    names.setString(
        std::string("|--- STEP:\n") +
        std::string("|    particles:\n") +
        std::string("|--\n") +
        std::string("|    FPS:\n") +
        std::string("|--\n") +
        std::string("|    G:\n") +
        std::string("|    theta:\n") +
        std::string("|    dt:\n") +
        std::string("|--\n") +
        std::string("|    physics:\n") +
        std::string("|    drawing:\n") +
        std::string("|    TOTAL:\n") +
        std::string("|--\n") +
        std::string("|    calc/frame:\n") +
        std::string("|    total calc:\n") +
        std::string("|--\n") +
        std::string("|    worst case:\n") +
        std::string("|    best case:\n") +
        std::string("|---------\n")
    );

    window->draw(names);

    sf::Text values;
    values.setPosition(left_offset + values_left_offset, top_offset + text_top_offset);
    values.setFont(font);
    values.setFillColor(sf::Color(200, 200, 200, 255));
    values.setOutlineColor(sf::Color::Black);

    std::stringstream valueStream;

    valueStream << std::left;
    valueStream << simulation_manager->get_step() << "\n";
    valueStream << simulation_manager->get_num_particles() << "\n\n";
    valueStream << std::fixed << std::setprecision(3) << simulation_manager->get_fps() << "\n\n";
    valueStream << std::scientific << std::setprecision(4) << simulation_manager->get_G() << "\n";
    valueStream << std::fixed << std::setprecision(1) << simulation_manager->get_theta() << "\n";
    valueStream << std::fixed << std::setprecision(2) << simulation_manager->get_dt() << "\n\n";
    valueStream << this->simulation_manager->get_elapsed_time_physics() << " ms\n";
    valueStream << this->simulation_manager->get_elapsed_time_graphics() << " ms\n";
    valueStream << this->simulation_manager->get_total_frame_time() << " ms\n\n";
    valueStream << std::setprecision(2) << std::scientific << static_cast<double>(this->simulation_manager->get_calculations_per_frame()) << "\n";
    valueStream << std::setprecision(2) << std::scientific << static_cast<double>(this->simulation_manager->get_total_interactions()) << "\n\n" << std::fixed;
    valueStream << std::setprecision(2) << simulation_manager->get_current_ratio_worst_case() << "x     (~" << simulation_manager->get_average_ratio_worst_case() << ")\n";
    valueStream << std::setprecision(2) << simulation_manager->get_current_ratio_best_case() << "x     (~" << simulation_manager->get_average_ratio_best_case() << ")\n";

    values.setString(valueStream.str());
    window->draw(values);
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
