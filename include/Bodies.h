#ifndef BODIES_H
#define BODIES_H

#include "Vec2.h"
#include <iostream>
#include <vector>
#include <utility>

class Bodies {
public:
    std::vector<Vec2> pos;
    std::vector<Vec2> vel;
    std::vector<Vec2> acc;

    std::vector<double> mass;
    std::vector<double> radius;

    std::vector<bool> to_be_deleted;

    unsigned size;
    unsigned width, height;

    Bodies(unsigned num_bodies);
    inline void set_size(unsigned width, unsigned height)
    {
        width = width;
        height = height;
    }

    void update(double dt);
    void resize(unsigned num_bodies);
    void clear();

    void remove_merged_bodies();
    void merge_bodies(unsigned keep_index, unsigned remove_index);

    double get_lowest_density() const;
    double get_highest_density() const;
    unsigned get_size() const;

    void add_force(unsigned index, const Vec2& force);
    void reset_force(unsigned index);

    void print() const;
    void print(unsigned index) const;
};

#endif // BODIES_H
