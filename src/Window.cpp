#include "Window.h"

Window::Window(int width, int height, const char* title)
    : width(width), height(height)
{
    window = new sf::RenderWindow(sf::VideoMode(width, height), title);
    window->setVerticalSyncEnabled(true);
    window->setPosition(sf::Vector2i(0, 0));
    window->setFramerateLimit(0);
}

Window::~Window()
{
    delete window;
}

void Window::clear()
{
    window->clear();
}

void Window::draw(const sf::Shape& shape)
{
    window->draw(shape);
}

void Window::draw(const sf::RectangleShape& shape)
{
    window->draw(shape);
}

void Window::draw(const sf::Vertex* line, int size, sf::PrimitiveType type)
{
    window->draw(line, size, type);
}

void Window::store_png(const std::string& filename)
{
    sf::Texture texture;
    texture.create(this->width, this->height);
    texture.update(*window);
    texture.copyToImage().saveToFile(filename);
}

void Window::display()
{
    window->display();
}

void Window::close()
{
    window->close();
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

void Window::handle_events(bool& toggle_pause, bool& toggle_draw_quadtree, bool& toggle_draw_vectors, bool& toggle_debug)
{
    sf::Event event;
    sf::RenderWindow& currentWindow = *window;  // Store a reference to the window

    while (currentWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            currentWindow.close();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Space)
            {
                toggle_pause = !toggle_pause;
            }
            else if (event.key.code == sf::Keyboard::Q)
            {
                toggle_draw_quadtree = !toggle_draw_quadtree;
            }
            else if (event.key.code == sf::Keyboard::Escape)
            {
                currentWindow.close();
            }
            else if (event.key.code == sf::Keyboard::V)
            {
                toggle_draw_vectors = !toggle_draw_vectors;
            }
            else if (event.key.code == sf::Keyboard::D)
            {
                toggle_debug = !toggle_debug;
            }
        }
    }
}
