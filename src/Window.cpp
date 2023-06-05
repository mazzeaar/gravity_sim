#include "Window.h"

Window::Window(int width, int height, const char* title)
{
    this->width = width;
    this->height = height;

    this->window = new sf::RenderWindow(sf::VideoMode(width, height), title);
    this->window->setFramerateLimit(60);

    this->circle = new sf::CircleShape();
    this->rect = new sf::RectangleShape();

    this->color = new sf::Color(0, 0, 0);
}

Window::~Window()
{
    delete this->window;
    delete this->circle;
    delete this->rect;
    delete this->color;
}

void Window::clear()
{
    this->window->clear();
}

void Window::draw(sf::Shape* shape)
{
    this->window->draw(*shape);
}

void Window::draw(sf::RectangleShape* shape)
{
    this->window->draw(*shape);
}

void Window::display()
{
    this->window->display();
}

void Window::close()
{
    this->window->close();
}

int Window::get_width()
{
    return this->width;
}

int Window::get_height()
{
    return this->height;
}

bool Window::is_open()
{
    return this->window->isOpen();
}

void Window::handle_events(bool& toggle_pause, bool& toggle_draw_quadtree)
{
    sf::Event event;
    while (this->window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            this->window->close();
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
                this->window->close();
            }
        }
    }
}
