#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "Bodies.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>


class QuadTree {
private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass;
    double mass;
    unsigned depth;

    int body_index = -1;

    std::unique_ptr<QuadTree> NW;
    std::unique_ptr<QuadTree> NE;
    std::unique_ptr<QuadTree> SW;
    std::unique_ptr<QuadTree> SE;

    void insert(unsigned index);
    bool subdivide();

    bool contains(unsigned index);
    inline bool is_leaf() { return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr; }

    double calculate_gravitational_force(double G, double mass1, double mass2, double squared_distance);
    void compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame);

public:
    std::shared_ptr<Bodies> bodies;

    QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right, bool is_root = false);
    QuadTree(std::shared_ptr<Bodies> bodies, double xmin, double ymin, double xmax, double ymax, bool is_root = false);

    ~QuadTree();

    void update(double theta, double G, double dt, unsigned long& calculations_per_frame);
    void get_bounding_rectangles(std::vector<sf::RectangleShape*>& bounding_boxes) const;
};

#endif // QUADTREE_H