#ifndef WINDOW_H    
#define WINDOW_H

#include <SFML/Graphics.hpp>

class Window {
private:
    sf::RenderWindow* window;
    sf::View* view;

    sf::Color* color;

    int width, height;

public:
    sf::CircleShape* circle;
    sf::RectangleShape* rect;

    Window(int width, int height, const char* title);
    ~Window();

    void clear();
    void draw(sf::Shape* shape);
    void display();
    void close();

    bool is_open();

    int get_width();
    int get_height();

    void handle_events();
};

#endif // WINDOW_H
