#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "Vec2.h"
#include "Body.h"
#include <iostream>
#include <SFML/Graphics.hpp>

class QuadTree {
private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass;
    double mass;

    Body* body; // NULL if no body contained
    QuadTree* NW, * NE, * SW, * SE; // children

    void compute_force(Body* body, double theta, double G);

public:
    QuadTree(Vec2 top_left, Vec2 bottom_right);
    QuadTree(double xmin, double ymin, double xmax, double ymax);

    ~QuadTree();

    bool contains(Body* body);
    void subdivide();

    void add_bodys(std::vector<Body*>& bodies);
    void insert(Body*& body);

    void update(std::vector<Body*>& bodies, double theta, double G, double dt);

    std::vector<sf::RectangleShape> get_bounding_rectangles() const;
};

#endif // QUADTREE_H