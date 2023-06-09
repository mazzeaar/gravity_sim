#include "QuadTree.h"

/*----------------------------------------
|        constructors/destructor         |
-----------------------------------------*/

QuadTree::QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right) :
    top_left(top_left), bottom_right(bottom_right)
{
    this->bodies = bodies;

    depth = 0;

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

void QuadTree::clear()
{
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;

    body_index = -1;
    center_of_mass = Vec2(0, 0);
    mass = 0.0;
    depth = 0;
}


/*----------------------------------------
|             boolean checks             |
-----------------------------------------*/

bool QuadTree::contains(unsigned index)
{
    return bodies->pos[index].x >= top_left.x
        && bodies->pos[index].x <= bottom_right.x
        && bodies->pos[index].y >= top_left.y
        && bodies->pos[index].y <= bottom_right.y;
}

bool QuadTree::subdivide()
{
    const double threshold = 0.1;

    if (bottom_right.x - top_left.x < threshold || bottom_right.y - top_left.y < threshold)
    {
        return false;
    }

    this->NW = std::make_unique<QuadTree>(bodies, top_left, (top_left + bottom_right) / 2.0);
    this->NE = std::make_unique<QuadTree>(bodies, Vec2((top_left.x + bottom_right.x) / 2.0, top_left.y), Vec2(bottom_right.x, (top_left.y + bottom_right.y) / 2.0));
    this->SW = std::make_unique<QuadTree>(bodies, Vec2(top_left.x, (top_left.y + bottom_right.y) / 2.0), Vec2((top_left.x + bottom_right.x) / 2.0, bottom_right.y));
    this->SE = std::make_unique<QuadTree>(bodies, (top_left + bottom_right) / 2.0, bottom_right);

    NW->depth = depth + 1;
    NE->depth = depth + 1;
    SW->depth = depth + 1;
    SE->depth = depth + 1;

    return true;
}


/*----------------------------------------
|                 insert                 |
-----------------------------------------*/

void QuadTree::insert(std::shared_ptr<Bodies> bodies)
{
    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        insert(i);
    }
}

void QuadTree::insert(unsigned index)
{
    // If the body is not in this quadrant, do not add it.
    if (!contains(index))
    {
        return;
    }

    if (this->mass == 0)
    {
        this->body_index = index;
        this->mass = bodies->mass[index];
        this->center_of_mass = bodies->pos[index];

        bodies->density[index] = depth;

        return;
    }

    this->center_of_mass = (this->center_of_mass * this->mass + bodies->pos[index] * bodies->mass[index]) / (this->mass + bodies->mass[index]);
    this->mass += bodies->mass[index];

    if (this->is_leaf())
    {
        if (this->body_index != -1)
        {
            if (this->subdivide())
            {
                // move present body to new quadrant
                unsigned old_index = this->body_index;
                this->body_index = -1;

                this->insert(old_index);
                this->insert(index);
            }
            else
            {
                // if we can't subdivide, we have to merge the bodies
                bodies->merge_bodies(this->body_index, index);
                return;
            }
        }

        // if no body is contained, we can just add the body
        this->body_index = index;
        return;
    }

    // if its not a leaf, we insert the body into the children
    NW->insert(index);
    NE->insert(index);
    SW->insert(index);
    SE->insert(index);
}

/*----------------------------------------
|                 update                 |
-----------------------------------------*/

void QuadTree::compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame)
{
    if (this->mass == 0 || this->body_index == index)
    {
        return;
    }

    Vec2 direction = this->center_of_mass - bodies->pos[index];
    double squared_distance = direction.squared_length();
    double squared_size = (bottom_right - top_left).squared_length();

    if (squared_size / squared_distance < theta || is_leaf())
    {
        ++calculations_per_frame;
        double force = calculate_gravitational_force(G, this->mass, bodies->mass[index], squared_distance);
        bodies->add_force(index, direction * force);
    }
    else
    {
        compute_force_on_children(index, theta, G, calculations_per_frame);
    }
}

double QuadTree::calculate_gravitational_force(double G, double mass1, double mass2, double squared_distance)
{
    const double epsilon_squared = 0.4; // softening factor, else force goes BRRRRRT
    return G * mass1 * mass2 / (squared_distance + epsilon_squared);
}

void QuadTree::compute_force_on_children(unsigned index, double theta, double G, unsigned long& calculations_per_frame)
{
    NW->compute_force(index, theta, G, calculations_per_frame);
    NE->compute_force(index, theta, G, calculations_per_frame);
    SW->compute_force(index, theta, G, calculations_per_frame);
    SE->compute_force(index, theta, G, calculations_per_frame);
}

void QuadTree::update(double theta, double G, double dt, unsigned long& calculations_per_frame)
{
    for (unsigned i = 0; i < bodies->get_size(); ++i)
    {
        compute_force(i, theta, G, calculations_per_frame);
    }

    bodies->remove_merged_bodies();
    bodies->update(dt);
}


/*----------------------------------------
|                 print                  |
-----------------------------------------*/

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
