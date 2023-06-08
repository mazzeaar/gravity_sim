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

    size_t size;

    // Constructor that resizes the vectors to hold a specific number of bodies
    Bodies(size_t num_bodies)
    {
        size = num_bodies;

        pos.resize(num_bodies, Vec2(0, 0));
        vel.resize(num_bodies, Vec2(0, 0));
        acc.resize(num_bodies, Vec2(0, 0));
        mass.resize(num_bodies, 0.0);
        radius.resize(num_bodies, 0.0);
        pressure.resize(num_bodies, std::numeric_limits<double>::max());
    }

    // Method to add a force to a specific body
    inline void add_force(size_t index, const Vec2& force)
    {
        acc[index] += force / mass[index];
    }

    // Method to reset the force on a specific body
    inline void reset_force(size_t index)
    {
        acc[index] = Vec2(0.0, 0.0);
    }

    // Method to update a specific body
    inline void update(size_t index, double dt)
    {
        vel[index] += acc[index] * dt;
        pos[index] += vel[index] * dt;
    }

    inline void update(double dt)
    {
        for (size_t i = 0; i < size; ++i)
        {
            vel[i] += acc[i] * dt;
            pos[i] += vel[i] * dt;
        }
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

    // Method to get the size of the bodies vector
    size_t get_size() { return size; }
};

class QuadTree {
private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass;
    double mass;
    double pressure_threshold;

    int body_index = -1;
    Bodies* bodies;

    QuadTree* NW, * NE, * SW, * SE; // children

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation
    void compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame);
    void insert(unsigned index);

public:
    QuadTree(Bodies* bodies, Vec2 top_left, Vec2 bottom_right);
    QuadTree(Bodies* bodies, double xmin, double ymin, double xmax, double ymax);

    ~QuadTree();

    // deletes all children and bodies
    void clear();

    // splits this quadtree into 4 children
    bool subdivide();

    // inserts one body into this quadtree
    inline void insert(Bodies* bodies)
    {
        for (unsigned i = 0; i < bodies->get_size(); ++i)
        {
            if (this->contains(i))
            {
                std::cout << "inserting body " << i << std::endl;

                insert(i);
            }
        }
    }

    // updates all bodies in this quadtree
    void update(double theta, double G, double dt, unsigned long& calculations_per_frame);

    // returns the bounding boxes of all bodies in this quadtree
    // => is used for drawing the quadtree structure and is extremely inefficient and expensive to run
    void get_bounding_rectangles(std::vector<sf::RectangleShape*>& bounding_boxes) const;

    // returns true if body is contained in this quadtree
    bool contains(unsigned index);

    // returns true if this quadtree has no children -> is a leaf
    inline bool is_leaf()
    {
        return NW == nullptr
            && NE == nullptr
            && SW == nullptr
            && SE == nullptr;
    }
};

#endif // QUADTREE_H