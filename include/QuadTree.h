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

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation
    void compute_force(Body* body, double theta, double G, unsigned long& calculations_per_frame);

public:
    QuadTree(Vec2 top_left, Vec2 bottom_right);
    QuadTree(double xmin, double ymin, double xmax, double ymax);

    ~QuadTree();

    // deletes all children and bodies
    void clear();

    // splits this quadtree into 4 children
    void subdivide();

    // inserts all bodies into this quadtree
    void add_bodies(std::vector<Body*>& bodies);

    // inserts one body into this quadtree
    void insert(Body*& body);

    // updates all bodies in this quadtree
    void update(std::vector<Body*>& bodies, double theta, double G, double dt, unsigned long& calculations_per_frame);

    // returns the bounding boxes of all bodies in this quadtree
    // => is used for drawing the quadtree structure and is extremely inefficient and expensive to run
    void get_bounding_rectangles(std::vector<sf::RectangleShape*>& bounding_boxes) const;

    // returns true if body is contained in this quadtree
    inline bool contains(Body* body)
    {
        return (body->pos.x >= top_left.x && body->pos.x <= bottom_right.x &&
            body->pos.y >= top_left.y && body->pos.y <= bottom_right.y);
    }

    // returns true if this quadtree has no children -> is a leaf
    inline bool is_leaf()
    {
        return (NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr);
    }
};

#endif // QUADTREE_H