#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "Vec2.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

struct Bodies {
    std::vector<Vec2> pos;
    std::vector<Vec2> vel;
    std::vector<Vec2> acc;

    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<double> pressure;

    // Constructor that resizes the vectors to hold a specific number of bodies
    Bodies(size_t num_bodies)
    {
        pos.resize(num_bodies, Vec2(0, 0));
        vel.resize(num_bodies, Vec2(0, 0));
        acc.resize(num_bodies, Vec2(0, 0));
        mass.resize(num_bodies, 0.0);
        radius.resize(num_bodies, 0.0);
        pressure.resize(num_bodies, std::numeric_limits<double>::max());
    }

    // Method to add a force to a specific body
    void add_force(size_t index, const Vec2& force)
    {
        acc[index] += force / mass[index];
    }

    // Method to reset the force on a specific body
    void reset_force(size_t index)
    {
        acc[index] = Vec2(0.0, 0.0);
    }

    // Method to update a specific body
    void update(size_t index, double dt)
    {
        vel[index] += acc[index] * dt;
        pos[index] += vel[index] * dt;
    }

    // Method to get the pressure of a specific body
    double get_pressure(size_t index)
    {
        return pressure[index];
    }

    // Method to reset the pressure of a specific body
    void reset_pressure(size_t index)
    {
        pressure[index] = std::numeric_limits<double>::max();
    }
};

class QuadTree {
private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass;
    double mass;
    double pressure_threshold;

    int body_index;
    Bodies* bodies;

    QuadTree* NW, * NE, * SW, * SE; // children

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation
    void compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame);

public:
    QuadTree(Vec2 top_left, Vec2 bottom_right, Bodies* bodies);
    QuadTree(double xmin, double ymin, double xmax, double ymax, Bodies* bodies);

    ~QuadTree();

    // deletes all children and bodies
    void clear();

    // splits this quadtree into 4 children
    bool subdivide();

    // inserts one body into this quadtree
    void insert(unsigned index);

    // updates all bodies in this quadtree
    void update(unsigned index, double theta, double G, double dt, unsigned long& calculations_per_frame);

    // returns the bounding boxes of all bodies in this quadtree
    // => is used for drawing the quadtree structure and is extremely inefficient and expensive to run
    void get_bounding_rectangles(std::vector<sf::RectangleShape*>& bounding_boxes) const;

    // returns true if body is contained in this quadtree
    inline bool contains(unsigned index)
    {
        return (bodies->pos[index].x >= top_left.x && bodies->pos[index].x <= bottom_right.x && bodies->pos[index].y >= top_left.y && bodies->pos[index].y <= bottom_right.y);
    }

    // returns true if this quadtree has no children -> is a leaf
    inline bool is_leaf()
    {
        return (NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr);
    }
};

#endif // QUADTREE_H