#ifndef BODIES_H
#define BODIES_H

#include "Vec2.h"
#include <iostream>
#include <vector>

class Bodies {
public:
    std::vector<Vec2> pos;
    std::vector<Vec2> vel;
    std::vector<Vec2> acc;

    std::vector<double> mass;
    std::vector<double> radius;

    double lowest_density;
    double highest_density;

    std::vector<bool> to_be_deleted;

    unsigned size;

    // Constructor that resizes the vectors to hold a specific number of bodies
    Bodies(unsigned num_bodies);

    void add_force(unsigned index, const Vec2& force);

    void reset_force(unsigned index);

    void update(double dt);

    void resize(unsigned num_bodies);

    void remove_merged_bodies();

    void merge_bodies(unsigned keep_index, unsigned remove_index);

    unsigned get_size();

    void print() const;

    void print(unsigned index) const;
};

#endif // BODIES_H
