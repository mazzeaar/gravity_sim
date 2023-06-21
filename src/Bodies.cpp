#include "Bodies.h"
#include <limits>
#include <cmath>

/*----------------------------------------
|               Constructor              |
-----------------------------------------*/

Bodies::Bodies(unsigned num_bodies)
{
    size = num_bodies;

    pos.resize(num_bodies);
    vel.resize(num_bodies);
    acc.resize(num_bodies, Vec2(0.0, 0.0));

    mass.resize(num_bodies, 0.0);
    radius.resize(num_bodies, 0.0);

    to_be_deleted.resize(num_bodies, false);
}


/*----------------------------------------
|                 getters                |
-----------------------------------------*/

unsigned Bodies::get_size() const
{
    return size;
}

double Bodies::get_lowest_density() const
{
    double lowest_density = std::numeric_limits<double>::max();
    for ( unsigned i = 0; i < size; ++i )
    {
        lowest_density = std::min(lowest_density, this->acc[i].length());
    }

    return lowest_density;
}

double Bodies::get_highest_density() const
{
    double highest_density = std::numeric_limits<double>::min();
    for ( unsigned i = 0; i < size; ++i )
    {
        highest_density = std::max(highest_density, this->acc[i].length());
    }

    return highest_density;
}

void Bodies::add_force(unsigned index, const Vec2& force)
{
    acc[index] += force / mass[index];
}

void Bodies::reset_force(unsigned index)
{
    acc[index] = Vec2(0.0, 0.0);
}


/*----------------------------------------
|            update/modify               |
-----------------------------------------*/

void Bodies::update(double dt)
{
    for ( unsigned i = 0; i < size; ++i )
    {
        vel[i] += acc[i] * (0.5 * dt);
        pos[i] += vel[i] * dt;
    }
}

void Bodies::resize(unsigned num_bodies)
{
    if ( num_bodies <= size )
        return;

    size = num_bodies;

    pos.resize(num_bodies);
    vel.resize(num_bodies);
    acc.resize(num_bodies, Vec2(0.0, 0.0));

    mass.resize(num_bodies, 0.0);
    radius.resize(num_bodies, 0.0);

    to_be_deleted.resize(num_bodies, false);
}

void Bodies::clear()
{
    size = 0;

    pos.clear();
    vel.clear();
    acc.clear();

    mass.clear();
    radius.clear();

    to_be_deleted.clear();
}

void Bodies::remove_merged_bodies()
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

            to_be_deleted.erase(to_be_deleted.begin() + i);
            --size;
        }
    }
}

void Bodies::merge_bodies(unsigned keep_index, unsigned remove_index)
{
    pos[keep_index] = (pos[keep_index] * mass[keep_index] + pos[remove_index] * mass[remove_index]) / (mass[keep_index] + mass[remove_index]);
    vel[keep_index] = (vel[keep_index] * mass[keep_index] + vel[remove_index] * mass[remove_index]) / (mass[keep_index] + mass[remove_index]);
    acc[keep_index] += acc[remove_index];

    mass[keep_index] += mass[remove_index];
    radius[keep_index] = std::pow(mass[keep_index], 1.0 / 3.0);

    to_be_deleted[remove_index] = true;
}


/*----------------------------------------
|                 print                  |
-----------------------------------------*/

void Bodies::print() const
{
    for ( unsigned i = 0; i < size; ++i )
    {
        print(i);
    }
}

void Bodies::print(unsigned index) const
{
    std::cout << "========================================" << std::endl;
    std::cout << "body: " << index << ":" << std::endl;
    std::cout << " - pos: " << pos[index] << std::endl;
    std::cout << " - vel: " << vel[index] << std::endl;
    std::cout << " - acc: " << acc[index] << std::endl;
    std::cout << " - mass: " << mass[index] << std::endl;
    std::cout << " - radius: " << radius[index] << std::endl;
    std::cout << " - to_be_deleted: " << to_be_deleted[index] << std::endl;
}