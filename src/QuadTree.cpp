#include "QuadTree.h"

QuadTree::QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right) :
    top_left(top_left), bottom_right(bottom_right)
{
    this->bodies = bodies;

    center_of_mass = Vec2(0, 0);
    mass = 0.0;

    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
}

QuadTree::QuadTree(std::shared_ptr<Bodies> bodies, double xmin, double ymin, double xmax, double ymax) :
    QuadTree(bodies, Vec2(xmin, ymin), Vec2(xmax, ymax))
{}

QuadTree::~QuadTree()
{
    if (NW != nullptr) delete NW;
    if (NE != nullptr) delete NE;
    if (SW != nullptr) delete SW;
    if (SE != nullptr) delete SE;
}

bool QuadTree::contains(unsigned index)
{
    return bodies->pos.at(index).x >= top_left.x
        && bodies->pos.at(index).x <= bottom_right.x
        && bodies->pos.at(index).y >= top_left.y
        && bodies->pos.at(index).y <= bottom_right.y;
}

void QuadTree::insert(std::shared_ptr<Bodies> bodies)
{
    if (bodies == nullptr || this->bodies != bodies)
    {
        std::cout << "Bodies is null" << std::endl;
        return;
    }

    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        if (this->contains(i))
        {
            insert(i);
        }
    }
}

void QuadTree::insert(unsigned index)
{
    if (!this->contains(index))
    {
        return;
    }

    if (body_index == -1)
    {
        body_index = index;
        center_of_mass = bodies->pos[index];
        mass = bodies->mass[index];
    }
    else
    {
        if (NW == nullptr && !this->subdivide())
        {
            bodies->pressure[index] = (center_of_mass - bodies->pos[index]).length() * mass;
            return;
        }
        else
        {
            if (NW->contains(index)) NW->insert(index);
            else if (NE->contains(index)) NE->insert(index);
            else if (SW->contains(index)) SW->insert(index);
            else if (SE->contains(index)) SE->insert(index);
            else
            {
                std::cout << "Body out of bounds :(" << std::endl;
                return;
            }
        }
    }

    center_of_mass = (center_of_mass * mass + bodies->pos[index] * bodies->mass[index]) / (mass + bodies->mass[index]);
    mass += bodies->mass[index];
}

bool QuadTree::subdivide()
{
    if ((this->bottom_right - this->top_left).length() < 1.0)
    {
        return false;
    }
    else
    {
        this->NW = new QuadTree(bodies, top_left, (top_left + bottom_right) / 2.0);
        this->NE = new QuadTree(bodies, Vec2((top_left.x + bottom_right.x) / 2.0, top_left.y), Vec2(bottom_right.x, (top_left.y + bottom_right.y) / 2.0));
        this->SW = new QuadTree(bodies, Vec2(top_left.x, (top_left.y + bottom_right.y) / 2.0), Vec2((top_left.x + bottom_right.x) / 2.0, bottom_right.y));
        this->SE = new QuadTree(bodies, (top_left + bottom_right) / 2.0, bottom_right);

        return true;
    }
}

void QuadTree::compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame)
{
    ++calculations_per_frame;
    double epsilon = 0.1; // softening factor to prevent force go brrrrr

    if (body_index == -1 || body_index == index)
    {
        return;
    }

    Vec2 direction = this->center_of_mass - bodies->pos[index];
    double distance = direction.length();

    // quadrant size = width = height
    double quadrant_size = (this->bottom_right.x - this->top_left.x);

    if (distance <= quadrant_size / theta) // Check if we are close enough
    {
        // We are close enough to this quadrant, so we will treat this quadrant as a single body.
        double force = (G * this->mass * bodies->mass[index]) / (distance * distance + epsilon * epsilon);
        bodies->add_force(index, direction.normalize() * force);
    }
    else
    {
        if (NW != nullptr) NW->compute_force(index, theta, G, calculations_per_frame);
        if (NE != nullptr) NE->compute_force(index, theta, G, calculations_per_frame);
        if (SW != nullptr) SW->compute_force(index, theta, G, calculations_per_frame);
        if (SE != nullptr) SE->compute_force(index, theta, G, calculations_per_frame);
    }
}

void QuadTree::update(double theta, double G, double dt, unsigned long& calculations_per_frame)
{
    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        this->compute_force(i, theta, G, calculations_per_frame);
    }

    bodies->update(dt);
}

void QuadTree::get_bounding_rectangles(std::vector<sf::RectangleShape*>& rectangles) const
{
    sf::RectangleShape* rect = new sf::RectangleShape(); // Create a dynamic object
    rect->setSize(sf::Vector2f(bottom_right.x - top_left.x, bottom_right.y - top_left.y));
    rect->setPosition(sf::Vector2f(top_left.x, top_left.y));
    rect->setFillColor(sf::Color::Transparent);
    rect->setOutlineColor(sf::Color::Green);
    rect->setOutlineThickness(1.0f);
    rectangles.push_back(rect);

    if (NW != nullptr)
    {
        NW->get_bounding_rectangles(rectangles);
        NE->get_bounding_rectangles(rectangles);
        SW->get_bounding_rectangles(rectangles);
        SE->get_bounding_rectangles(rectangles);
    }
}

void QuadTree::clear()
{
    if (NW != nullptr)
    {
        delete NW;
        NW = nullptr;
    }
    if (NE != nullptr)
    {
        delete NE;
        NE = nullptr;
    }
    if (SW != nullptr)
    {
        delete SW;
        SW = nullptr;
    }
    if (SE != nullptr)
    {
        delete SE;
        SE = nullptr;
    }

    if (bodies != nullptr)
    {
        bodies = nullptr;
    }

    body_index = -1;
    center_of_mass = Vec2(0, 0);
    mass = 0.0;
}