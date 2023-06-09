#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "Vec2.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>

struct Bodies {
public:
    std::vector<Vec2> pos;
    std::vector<Vec2> vel;
    std::vector<Vec2> acc;

    std::vector<double> mass;
    std::vector<double> radius;
    std::vector<double> pressure;

    std::vector<unsigned> density;

    std::vector<bool> to_be_deleted;

    unsigned size;

    // Constructor that resizes the vectors to hold a specific number of bodies
    Bodies(unsigned num_bodies)
    {
        size = num_bodies;

        pos.resize(num_bodies);
        vel.resize(num_bodies);
        acc.resize(num_bodies, Vec2(0.0, 0.0));

        mass.resize(num_bodies, 0.0);
        radius.resize(num_bodies, 0.0);
        pressure.resize(num_bodies, 0.0);
        density.resize(num_bodies, 0);

        to_be_deleted.resize(num_bodies, false);
    }

    // acceleration += force / mass
    inline void add_force(unsigned index, const Vec2& force)
    {
        acc[index] += force / mass[index];
    }

    // acceleration = (0, 0)
    inline void reset_force(unsigned index)
    {
        acc[index] = Vec2(0.0, 0.0);
    }

    inline void resize(unsigned num_bodies)
    {
        if ( num_bodies <= size ) return;

        size = num_bodies;

        pos.resize(num_bodies);
        vel.resize(num_bodies);
        acc.resize(num_bodies, Vec2(0.0, 0.0));

        mass.resize(num_bodies, 0.0);
        radius.resize(num_bodies, 0.0);
        pressure.resize(num_bodies, 0.0);
        density.resize(num_bodies, 0);

        to_be_deleted.resize(num_bodies, false);
    }

    inline void remove_merged_bodies()
    {
        for ( unsigned i = 0; i < size; ++i )
        {
            if ( to_be_deleted[i] )
            {
                pos.erase(pos.begin() + i);
                vel.erase(vel.begin() + i);
                acc.erase(acc.begin() + i);

                mass.erase(mass.begin() + i);
                radius.erase(radius.begin() + i);
                pressure.erase(pressure.begin() + i);
                density.erase(density.begin() + i);

                to_be_deleted.erase(to_be_deleted.begin() + i);
                --size;
            }
        }
    }

    inline void merge_bodies(unsigned keep_index, unsigned remove_index)
    {
        pos[keep_index] = (pos[keep_index] * mass[keep_index] + pos[remove_index] * mass[remove_index]) / (mass[keep_index] + mass[remove_index]);
        vel[keep_index] = (vel[keep_index] * mass[keep_index] + vel[remove_index] * mass[remove_index]) / (mass[keep_index] + mass[remove_index]);
        acc[keep_index] += acc[remove_index];

        mass[keep_index] += mass[remove_index];
        radius[keep_index] = std::pow(mass[keep_index], 1.0 / 3.0);

        to_be_deleted[remove_index] = true;
    }


    inline void update(double dt)
    {
        for ( unsigned i = 0; i < size; ++i )
        {
            if ( mass[i] == 0.0 ) continue;

            // Update velocity half a step forward using the current acceleration and the time step
            vel[i] = vel[i] + acc[i] * (0.5 * dt);

            // Update position using the updated velocity and the time step
            pos[i] = pos[i] + vel[i] * dt;

            // Reset the force before each iteration
            reset_force(i);
        }
    }


    inline double get_pressure(unsigned index) { return pressure[index]; }
    inline void reset_pressure(unsigned index) {}
    inline unsigned get_size() { return size; }

    inline void print() const
    {
        for ( unsigned i = 0; i < size; ++i )
        {
            print(i);
        }
    }

    inline void print(unsigned index) const
    {
        std::cout << "========================================" << std::endl;
        std::cout << "body: " << index << ": " << std::endl;
        std::cout << " - pos: " << pos[index] << std::endl;
        std::cout << " - vel: " << vel[index] << std::endl;
        std::cout << " - acc: " << acc[index] << std::endl;
        std::cout << " - mass: " << mass[index] << std::endl;
        std::cout << " - radius: " << radius[index] << std::endl;
        std::cout << " - pressure: " << pressure[index] << std::endl;
        std::cout << " - to_be_deleted: " << to_be_deleted[index] << std::endl;
    }
};


class QuadTree {

private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass;
    double mass;
    double pressure_threshold;

    unsigned depth;
    long total_bodies;

    int body_index = -1;

    std::unique_ptr<QuadTree> NW;
    std::unique_ptr<QuadTree> NE;
    std::unique_ptr<QuadTree> SW;
    std::unique_ptr<QuadTree> SE;

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation
    double calculate_gravitational_force(double G, double mass1, double mass2, double squared_distance);

    // computes the force exerted on body by this quadtree using the Barnes-Hut approximation
    void compute_force_on_children(unsigned index, double theta, double G, unsigned long& calculations_per_frame);

public:

    QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right);
    QuadTree(std::shared_ptr<Bodies> bodies, double xmin, double ymin, double xmax, double ymax);

    ~QuadTree();
    void clear();
    void compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame);
    std::shared_ptr<Bodies> bodies;

    // splits this quadtree into 4 children
    bool subdivide();
    void insert(std::shared_ptr<Bodies> bodies);
    void insert(unsigned index);

    bool contains(unsigned index);
    inline bool is_leaf() { return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr; }
    inline long get_total_bodies() { return total_bodies; }

    // updates all bodies in this quadtree
    void update(double theta, double G, double dt, unsigned long& calculations_per_frame);

    // returns the bounding boxes of all bodies in this quadtree
    void get_bounding_rectangles(std::vector<sf::RectangleShape*>& bounding_boxes) const;
};

#endif // QUADTREE_H