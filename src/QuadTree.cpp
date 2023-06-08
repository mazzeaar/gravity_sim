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
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
}

bool QuadTree::contains(unsigned index)
{
    return bodies->pos.at(index).x >= top_left.x
        && bodies->pos.at(index).x <= bottom_right.x
        && bodies->pos.at(index).y >= top_left.y
        && bodies->pos.at(index).y <= bottom_right.y;
}

void QuadTree::insert(std::shared_ptr<Bodies>& bodies)
{
    if (bodies == nullptr || this->bodies != bodies)
    {
        std::string error = "QuadTree::insert(std::shared_ptr<Bodies> bodies) : bodies == nullptr || this->bodies != bodies";
        std::string error_location = "Path: src/QuadTree.cpp";
        // append the values of the variables to the error message
        std::string error_details = "bodies = " + std::to_string((long long) bodies.get()) + ", this->bodies = " + std::to_string((long long) this->bodies.get());
        throw std::invalid_argument(error + "\n" + error_location + "\n" + error_details);
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
        this->NW = std::make_shared<QuadTree>(bodies, top_left, (top_left + bottom_right) / 2.0);
        this->NE = std::make_shared<QuadTree>(bodies, Vec2((top_left.x + bottom_right.x) / 2.0, top_left.y), Vec2(bottom_right.x, (top_left.y + bottom_right.y) / 2.0));
        this->SW = std::make_shared<QuadTree>(bodies, Vec2(top_left.x, (top_left.y + bottom_right.y) / 2.0), Vec2((top_left.x + bottom_right.x) / 2.0, bottom_right.y));
        this->SE = std::make_shared<QuadTree>(bodies, (top_left + bottom_right) / 2.0, bottom_right);

        return true;
    }
}

void QuadTree::compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame)
{
    if (this->body_index == index)
    {
        std::cout << "not computing force on self" << std::endl;
    }

    double epsilon = 0.1; // softening factor to prevent force go brrrrr

    Vec2 direction = this->center_of_mass - bodies->pos[index];
    double distance = direction.length();

    if (distance == 0.0)
    {
        std::cout << "distance == 0.0" << std::endl;
        return;
    }

    // quadrant size = width = height
    double quadrant_size = this->bottom_right.x - this->top_left.x;

    if (quadrant_size / distance < theta || this->NW == nullptr) // Check if we are close enough
    {
        ++calculations_per_frame;
        double force = (G * this->mass * bodies->mass[index]) / (distance * distance + epsilon * epsilon);
        bodies->add_force(index, direction.normalize() * force);
    }
    else
    {
        if (NW != nullptr && NW->contains(index)) NW->compute_force(index, theta, G, calculations_per_frame);
        if (NE != nullptr && NE->contains(index)) NE->compute_force(index, theta, G, calculations_per_frame);
        if (SW != nullptr && SW->contains(index)) SW->compute_force(index, theta, G, calculations_per_frame);
        if (SE != nullptr && SE->contains(index)) SE->compute_force(index, theta, G, calculations_per_frame);
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
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;

    body_index = -1;
    center_of_mass = Vec2(0, 0);
    mass = 0.0;
}