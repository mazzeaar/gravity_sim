#ifndef WINDOW_H    
#define WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Window {
private:
    sf::RenderWindow* window;
    sf::View* view;

    sf::Color* color;

    int width, height;

public:
    bool paused;

    Window(int width, int height, const char* title);
    ~Window();

    void clear();

    void draw(const sf::Shape& shape);
    void draw(const sf::RectangleShape& shape);
    void draw(const sf::Vertex* line, int size, sf::PrimitiveType type);
    void draw(const sf::VertexArray& vertices);
    void draw(const sf::VertexArray& vertices, sf::RenderStates states);

    void display();
    void close();
    void store_png(const std::string& filename);

    bool is_open();

    int get_width();
    int get_height();

    void handle_events(bool& toggle_pause, bool& toggle_draw_quadtree,
        bool& toggle_draw_vectors, bool& toggle_debug,
        double& dt, double& G);
};

#endif // WINDOW_H
