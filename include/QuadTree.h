#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "Vec2.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

struct Bodies {
public:
    std::vector<Vec2> pos;
    std::vector<Vec2> vel;
    std::vector<Vec2> acc;

    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<double> pressure;

    unsigned size;

    // Constructor that resizes the vectors to hold a specific number of bodies
    Bodies(unsigned num_bodies)
    {
        size = num_bodies;

        pos.resize(num_bodies);
        vel.resize(num_bodies);
        acc.resize(num_bodies);
        mass.resize(num_bodies, 0.0);
        radius.resize(num_bodies, 0.0);
        pressure.resize(num_bodies, std::numeric_limits<double>::max());
    }

    // Method to add a force to a specific body
    inline void add_force(unsigned index, const Vec2& force)
    {
        acc[index] += force / mass[index];
    }

    // Method to reset the force on a specific body
    inline void reset_force(unsigned index)
    {
        acc[index] = Vec2(0.0, 0.0);
    }

    inline void resize(unsigned num_bodies)
    {
        if (num_bodies <= size) return;

        size = num_bodies;

        pos.resize(num_bodies);
        vel.resize(num_bodies);
        acc.resize(num_bodies);
        mass.resize(num_bodies, 0.0);
        radius.resize(num_bodies, 0.0);
        pressure.resize(num_bodies, std::numeric_limits<double>::max());
    }

    inline void update(double dt)
    {
        for (unsigned i = 0; i < size; ++i)
        {
            vel[i] += acc[i] * dt;
            pos[i] += vel[i] * dt;

            reset_force(i);
        }
    }

    double get_pressure(unsigned index) { return pressure[index]; }
    void reset_pressure(unsigned index) { pressure[index] = std::numeric_limits<double>::max(); }
    unsigned get_size() { return size; }

    void print() const
    {
        for (unsigned i = 0; i < size; ++i)
        {
            print(i);
        }
    }

    void print(unsigned index) const
    {
        std::cout << "===> body " << index << ": " << std::endl;
        std::cout << "     pos: " << pos[index] << std::endl;
        std::cout << "     vel: " << vel[index] << std::endl;
        std::cout << "     acc: " << acc[index] << std::endl;
        std::cout << "     mass: " << mass[index] << std::endl;
        std::cout << "     radius: " << radius[index] << std::endl;
        std::cout << "     pressure: " << pressure[index] << std::endl;
    }
};

class QuadTree {
private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass;
    double mass;
    double pressure_threshold;

    int body_index = -1;
    std::shared_ptr<Bodies> bodies;

    QuadTree* NW, * NE, * SW, * SE; // children

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation
    void compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame);

public:
    QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right);
    QuadTree(std::shared_ptr<Bodies> bodies, double xmin, double ymin, double xmax, double ymax);

    ~QuadTree();

    // deletes all children and bodies
    void clear();

    // splits this quadtree into 4 children
    bool subdivide();

    // inserts one body into this quadtree
    void insert(std::shared_ptr<Bodies> bodies);
    void insert(unsigned index);

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