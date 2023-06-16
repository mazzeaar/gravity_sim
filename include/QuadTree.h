#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "Bodies.h"
#include "Vec2.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <queue>
#include <future>

class QuadTree {
private:
    Vec2 top_left, bottom_right; //bounding box

    Vec2 center_of_mass = Vec2(0, 0);
    double mass = 0.0;
    unsigned depth = 0;

    int body_index = -1;

    std::unique_ptr<QuadTree> NW;
    std::unique_ptr<QuadTree> NE;
    std::unique_ptr<QuadTree> SW;
    std::unique_ptr<QuadTree> SE;

    // 0, 255, 0, 100
    sf::Color color = sf::Color(0, 255, 0, 100);

    std::shared_ptr<sf::VertexArray> rectangles;

    void insert(unsigned index);
    void reset_helper(std::vector<unsigned>& moved_bodies, Vec2 top_left, Vec2 bottom_right);

    bool subdivide();
    QuadTree* get_child_quadrant(unsigned index);

    void add_subdivision_bounds();
    void add_root_bounds();

    bool contains(unsigned index) const;
    inline bool is_leaf() const { return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr; }

    double calculate_gravitational_force(double G, double mass1, double mass2, double squared_distance) const;
    void compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame);
    void compute_force_recursive(unsigned index, double theta_squared, double G, unsigned long& calculations_per_frame);

public:
    std::shared_ptr<Bodies> bodies;

    QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right, std::shared_ptr<sf::VertexArray> rectangles = nullptr, bool is_root = false);
    QuadTree(std::shared_ptr<Bodies> bodies, double xmin, double ymin, double xmax, double ymax, std::shared_ptr<sf::VertexArray> rectangles = nullptr, bool is_root = false);

    ~QuadTree();

    void update(double theta, double G, double dt, unsigned long& calculations_per_frame);
    void reset(Vec2 top_left, Vec2 bottom_right);

    inline sf::VertexArray* get_bounding_rectangles() const { return rectangles.get(); }
    inline Vec2 get_center_of_mass() const { return center_of_mass; }
    inline void get_size(Vec2& top_left, Vec2& bottom_right) const { top_left = this->top_left; bottom_right = this->bottom_right; }
};

#endif // QUADTREE_H