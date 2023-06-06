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
    double pressure_threshold;

    Body* body; // NULL if no body contained
    QuadTree* NW, * NE, * SW, * SE; // children

    void compute_force(Body* body, double theta, double G, unsigned long& calculations_per_frame);

public:
    QuadTree(Vec2 top_left, Vec2 bottom_right);
    QuadTree(double xmin, double ymin, double xmax, double ymax);

    ~QuadTree();
    void clear();

    bool contains(Body* body);
    bool is_leaf();

    void subdivide();

    void add_bodies(std::vector<Body*>& bodies);
    void insert(Body*& body);

    void update(std::vector<Body*>& bodies, double theta, double G, double dt, unsigned long& calculations_per_frame);

    void get_bounding_rectangles(std::vector<sf::RectangleShape*>& bounding_boxes) const;
};

#endif // QUADTREE_H