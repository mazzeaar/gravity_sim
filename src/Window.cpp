#include "Window.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

Window::Window(int width, int height, const char* title, std::shared_ptr<SimulationManager> simulation_manager)
    : width(width), height(height)
{
    this->simulation_manager = simulation_manager;
    this->bodies = this->simulation_manager->get_bodies();

    this->toggle_tracking = true;
    this->isDragging = false;
    this->zoomFactor = 1.0;

    // WINDOW
    window = new sf::RenderWindow(sf::VideoMode(width, height), title);

    window->setPosition(sf::Vector2i(0, 0));
    window->setFramerateLimit(120);
    window->setVerticalSyncEnabled(true);

    // VIEWS
    view = new sf::View(sf::FloatRect(0, 0, width, height));
    ui_view = new sf::View(sf::FloatRect(0, 0, width, height));

    // FONT
    font = sf::Font();
    font.loadFromFile("assets/fonts/montserrat/Montserrat-Regular.otf");
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
    window->clear();
    window->setView(*view);

    handle_events();

    if ( toggle_tracking )
    {
        reset_view();
    }

    draw_everything();
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

void Window::draw_everything()
{
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
        draw_ui();
    }
}

void Window::draw_bodies()
{
    double interpolation_cutoff = 0.2;

    sf::Color low_density_color = sf::Color(0, 128, 255);     // Light blue
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

    double* cosValues = new double[bodies->get_size()];
    double* sinValues = new double[bodies->get_size()];

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double angle = atan2(bodies->vel[i].y, bodies->vel[i].x);
        cosValues[i] = cos(angle);
        sinValues[i] = sin(angle);
    }

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        double length = bodies->vel[i].length() * lineLengthMultiplier;

        sf::Vector2f startPos(bodies->pos[i].x, bodies->pos[i].y);
        sf::Vector2f endPos(
            bodies->pos[i].x + cosValues[i] * length,
            bodies->pos[i].y + sinValues[i] * length
        );

        lines.append(sf::Vertex(startPos, sf::Color(255, 255, 255, 150)));
        lines.append(sf::Vertex(endPos, sf::Color(255, 255, 255, 150)));
    }

    window->draw(lines);

    delete[] cosValues;
    delete[] sinValues;

    lines.clear();
}

void Window::draw_quadtree_bounds()
{
    sf::VertexArray* lines = simulation_manager->get_bounding_rectangles();

    window->draw(*lines);
}

void Window::draw_ui()
{
    window->setView(*ui_view);

    unsigned left_offset = 20;
    unsigned top_offset = 20;
    unsigned spacing = 20;

    sf::Text statusText;
    statusText.setPosition(left_offset, top_offset);
    statusText.setFont(font);
    statusText.setOutlineColor(sf::Color::Black);
    statusText.setFillColor(simulation_manager->get_toggle_paused() ? sf::Color::Red : sf::Color::Green);
    statusText.setString(simulation_manager->get_toggle_paused() ? "PAUSED" : "RUNNING");

    sf::Text names;
    names.setPosition(left_offset, top_offset + statusText.getLocalBounds().height + spacing);
    names.setFont(font);
    names.setFillColor(sf::Color::White);
    names.setOutlineColor(sf::Color::Black);

    std::stringstream namesStream;
    namesStream << "|--- STEP:\n"
        << "|    particles:\n"
        << "|--\n"
        << "|    FPS:\n"
        << "|--\n"
        << "|    G:\n"
        << "|    theta:\n"
        << "|    dt:\n"
        << "|--\n"
        << "|    physics:\n"
        << "|    drawing:\n"
        << "|    TOTAL:\n"
        << "|--\n"
        << "|    calc/frame:\n"
        << "|    total calc:\n"
        << "|--\n"
        << "|    worst case:\n"
        << "|    best case:\n"
        << "|---------\n";

    names.setString(namesStream.str());

    sf::Text values;
    values.setPosition(left_offset + names.getLocalBounds().width + spacing, top_offset + statusText.getLocalBounds().height + spacing);
    values.setFont(font);
    values.setFillColor(sf::Color(200, 200, 200, 255));
    values.setOutlineColor(sf::Color::Black);

    std::stringstream valueStream;

    bool toggleDrawQuadtree = simulation_manager->get_toggle_draw_quadtree();
    bool toggleDrawVectors = simulation_manager->get_toggle_draw_vectors();

    valueStream << std::left
        << simulation_manager->get_step() << "\n"
        << simulation_manager->get_num_particles() << "\n\n"
        << std::fixed << std::setprecision(3) << simulation_manager->get_fps() << "\n\n"
        << std::scientific << std::setprecision(4) << simulation_manager->get_G() << "\n"
        << std::fixed << std::setprecision(1) << simulation_manager->get_theta() << "\n"
        << std::fixed << std::setprecision(2) << simulation_manager->get_dt() << "\n\n"
        << simulation_manager->get_elapsed_time_physics() << " ms\n"
        << simulation_manager->get_elapsed_time_graphics() << " ms\n"
        << simulation_manager->get_total_frame_time() << " ms\n\n"
        << std::setprecision(2) << std::scientific << static_cast<double>(simulation_manager->get_interactions_per_frame()) << "\n"
        << std::setprecision(2) << std::scientific << static_cast<double>(simulation_manager->get_total_interactions()) << "\n\n" << std::fixed
        << std::setprecision(2) << simulation_manager->get_current_ratio_best_case() << "x     (~" << simulation_manager->get_average_ratio_best_case() << ")\n"
        << std::setprecision(2) << simulation_manager->get_current_ratio_worst_case() << "x     (~" << simulation_manager->get_average_ratio_worst_case() << ")\n";

    values.setString(valueStream.str());

    sf::Text toggleText;
    toggleText.setPosition(left_offset, top_offset + statusText.getLocalBounds().height + names.getLocalBounds().height + spacing);
    toggleText.setFont(font);
    toggleText.setCharacterSize(20);
    toggleText.setOutlineColor(sf::Color::Black);
    toggleText.setFillColor(sf::Color::White);
    toggleText.setString("DRAW QUADTREE:\nDRAW VECTORS:\nTRACKING:\n");

    sf::Text toggleQuadtree;
    toggleQuadtree.setPosition(left_offset + names.getLocalBounds().width + spacing, toggleText.getPosition().y);
    toggleQuadtree.setFont(font);
    toggleQuadtree.setCharacterSize(20);
    toggleQuadtree.setOutlineColor(sf::Color::Black);
    toggleQuadtree.setFillColor(toggleDrawQuadtree ? sf::Color::Green : sf::Color::Red);
    toggleQuadtree.setString(toggleDrawQuadtree ? "TRUE" : "FALSE");

    sf::Text toggleVectors;
    toggleVectors.setPosition(left_offset + names.getLocalBounds().width + spacing, toggleQuadtree.getPosition().y + toggleQuadtree.getLocalBounds().height + 8);
    toggleVectors.setFont(font);
    toggleVectors.setCharacterSize(20);
    toggleVectors.setOutlineColor(sf::Color::Black);
    toggleVectors.setFillColor(toggleDrawVectors ? sf::Color::Green : sf::Color::Red);
    toggleVectors.setString(toggleDrawVectors ? "TRUE" : "FALSE");

    sf::Text toggleTracking;
    toggleTracking.setPosition(left_offset + names.getLocalBounds().width + spacing, toggleVectors.getPosition().y + toggleVectors.getLocalBounds().height + 8);
    toggleTracking.setFont(font);
    toggleTracking.setCharacterSize(20);
    toggleTracking.setOutlineColor(sf::Color::Black);
    toggleTracking.setFillColor(this->toggle_tracking ? sf::Color::Green : sf::Color::Red);
    toggleTracking.setString(this->toggle_tracking ? "TRUE" : "FALSE");

    // Combine similar draw operations
    window->draw(statusText);
    window->draw(names);
    window->draw(values);
    window->draw(toggleText);
    window->draw(toggleQuadtree);
    window->draw(toggleVectors);
    window->draw(toggleTracking);

    window->setView(*view);
}


/*--------------------
|    event handling   |
---------------------*/

void Window::handle_events()
{
    sf::Event event;

    while ( this->window->pollEvent(event) )
    {
        if ( event.type == sf::Event::Closed )
        {
            this->window->close();
        }
        else if ( event.type == sf::Event::KeyPressed )
        {
            settings_events(event);
        }
        else if ( event.type == sf::Event::MouseWheelScrolled )
        {
            handle_mouse_wheel(event.mouseWheelScroll);
        }
        else if ( event.type == sf::Event::MouseButtonPressed )
        {
            handle_mouse_press(event.mouseButton);
        }
        else if ( event.type == sf::Event::MouseButtonReleased )
        {
            handle_mouse_release(event.mouseButton);
        }
        else if ( event.type == sf::Event::MouseMoved )
        {
            handle_mouse_move(event.mouseMove);
        }
    }
}

void Window::settings_events(sf::Event& event)
{
    if ( event.key.code == sf::Keyboard::Escape )
    {
        this->window->close();
    }

    else if ( event.key.code == sf::Keyboard::Space )
    {
        simulation_manager->toggle_pause();
    }

    else if ( event.key.code == sf::Keyboard::Q )
    {
        simulation_manager->toggle_draw_quadtree();
    }

    else if ( event.key.code == sf::Keyboard::V )
    {
        simulation_manager->toggle_draw_vectors();
    }

    else if ( event.key.code == sf::Keyboard::D )
    {
        simulation_manager->toggle_debug_info();
    }

    else if ( event.key.code == sf::Keyboard::Right )
    {
        simulation_manager->increase_dt();
    }

    else if ( event.key.code == sf::Keyboard::Left )
    {
        simulation_manager->decrease_dt();
    }

    else if ( event.key.code == sf::Keyboard::Up )
    {
        simulation_manager->increase_G();
    }

    else if ( event.key.code == sf::Keyboard::Down )
    {
        simulation_manager->decrease_G();
    }

    else if ( event.key.code == sf::Keyboard::Enter )
    {
        this->view->setCenter(sf::Vector2f(this->width / 2.0, this->height / 2.0));

        this->zoomFactor = 1.0;
        this->isDragging = false;
        this->lastMousePos = sf::Vector2i(0, 0);
    }

    else if ( event.key.code == sf::Keyboard::T )
    {
        this->toggle_tracking = !this->toggle_tracking;
    }
}

void Window::handle_mouse_wheel(const sf::Event::MouseWheelScrollEvent& event)
{
    toggle_tracking = false;

    if ( event.delta > 0 )
    {
        zoomFactor *= 0.9;
    }
    else if ( event.delta < 0 )
    {
        zoomFactor *= 1.1;
    }

    view->setSize(sf::Vector2f(width / zoomFactor, height / zoomFactor));
    window->setView(*view);
}

void Window::handle_mouse_press(const sf::Event::MouseButtonEvent& event)
{
    toggle_tracking = false;
    if ( event.button == sf::Mouse::Left )
    {
        isDragging = true;
        lastMousePos = sf::Vector2i(event.x, event.y);
    }
}

void Window::handle_mouse_release(const sf::Event::MouseButtonEvent& event)
{
    toggle_tracking = false;
    if ( event.button == sf::Mouse::Left )
    {
        isDragging = false;
    }
}

void Window::handle_mouse_move(const sf::Event::MouseMoveEvent& event)
{
    if ( isDragging )
    {
        sf::Vector2i mousePos(event.x, event.y);
        sf::Vector2f delta = sf::Vector2f(lastMousePos - mousePos);

        view->move(sf::Vector2f(delta.x / zoomFactor, delta.y / zoomFactor));
        window->setView(*view);

        lastMousePos = mousePos;
    }
}

void Window::reset_view()
{
    view->setCenter(sf::Vector2f(simulation_manager->get_center_of_mass().x, simulation_manager->get_center_of_mass().y));

    Vec2 top_left, bottom_right;
    simulation_manager->get_quadtree_size(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
    view->setSize(sf::Vector2f(bottom_right.x * 1.6, bottom_right.y * 1.6));
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
