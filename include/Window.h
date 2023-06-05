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

    bool paused;

    Window(int width, int height, const char* title);
    ~Window();

    void clear();

    void draw(sf::Shape* shape);
    void draw(sf::RectangleShape* shape);
    void draw(sf::Vertex* line, int size, sf::PrimitiveType type);

    void display();
    void close();

    bool is_open();

    int get_width();
    int get_height();

    void handle_events(bool& toggle_pause, bool& toggle_draw_quadtree, bool& toggle_draw_vectors);
};

#endif // WINDOW_H
