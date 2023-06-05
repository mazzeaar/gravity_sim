#include "QuadTree.h"

QuadTree::QuadTree(Vec2 top_left, Vec2 bottom_right)
{
    this->top_left = top_left;
    this->bottom_right = bottom_right;

    center_of_mass = Vec2(0, 0);
    mass = 0.0;

    body = nullptr;
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
}

QuadTree::QuadTree(double xmin, double ymin, double xmax, double ymax) :
    QuadTree(Vec2(xmin, ymin), Vec2(xmax, ymax)) {}

QuadTree::~QuadTree()
{
    if (NW != nullptr) delete NW;
    if (NE != nullptr) delete NE;
    if (SW != nullptr) delete SW;
    if (SE != nullptr) delete SE;
}

bool QuadTree::contains(Body* body)
{
    return (body->pos.x >= top_left.x && body->pos.x <= bottom_right.x &&
        body->pos.y >= top_left.y && body->pos.y <= bottom_right.y);
}

void QuadTree::insert(Body*& body)
{
    if (!this->contains(body)) {
        //delete body;
        return;
    }

    if (this->body == nullptr) {
        this->body = body;
        center_of_mass = body->pos;
        mass = body->mass;
    }
    else {
        if (NW == nullptr) this->subdivide();

        if (NW->contains(body)) NW->insert(body);
        else if (NE->contains(body)) NE->insert(body);
        else if (SW->contains(body)) SW->insert(body);
        else if (SE->contains(body)) SE->insert(body);
        else {
            std::cout << "Body out of bounds" << std::endl;
            return;
        }

        center_of_mass = (center_of_mass * mass + body->pos * body->mass) / (mass + body->mass);
        mass += body->mass;
    }
}

void QuadTree::subdivide()
{
    this->NW = new QuadTree(top_left, (top_left + bottom_right) / 2.0);
    this->NE = new QuadTree(Vec2((top_left.x + bottom_right.x) / 2.0, top_left.y), Vec2(bottom_right.x, (top_left.y + bottom_right.y) / 2.0));
    this->SW = new QuadTree(Vec2(top_left.x, (top_left.y + bottom_right.y) / 2.0), Vec2((top_left.x + bottom_right.x) / 2.0, bottom_right.y));
    this->SE = new QuadTree((top_left + bottom_right) / 2.0, bottom_right);
}

void QuadTree::compute_force(Body* body, double theta, double G)
{
    if (this->body == nullptr || this->body == body) {
        return;
    }

    Vec2 direction = this->center_of_mass - body->pos;
    double distance = direction.length();

    double s = this->bottom_right.x - this->top_left.x; // size of the quadrant

    if (s / distance < theta || this->NW == nullptr) {
        double magnitude = G * this->mass * body->mass / (distance * distance * distance);
        Vec2 force = direction * magnitude;
        body->apply_force(force); // Accumulate the force on the body
    }
    else {
        NW->compute_force(body, theta, G);
        NE->compute_force(body, theta, G);
        SW->compute_force(body, theta, G);
        SE->compute_force(body, theta, G);
    }
}

void QuadTree::update(std::vector<Body*>& bodies, double theta, double G, double dt)
{
    // Compute forces for each body
    for (Body* body : bodies) {
        body->reset_force(); // Reset the accumulated force for each body
        this->compute_force(body, theta, G);
    }

    // Update positions and velocities for each body
    for (Body* body : bodies) {
        body->update(dt);
    }
}

void QuadTree::add_bodys(std::vector<Body*>& bodies)
{
    for (Body* body : bodies) {
        this->insert(body);
    }
}

std::vector<sf::RectangleShape> QuadTree::getBoundingRectangles() const {
    std::vector<sf::RectangleShape> rectangles;

    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(bottom_right.x - top_left.x, bottom_right.y - top_left.y));
    rect.setPosition(sf::Vector2f(top_left.x, top_left.y));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(1.0f);
    rectangles.push_back(rect);

    if (NW != nullptr) {
        std::vector<sf::RectangleShape> nwRectangles = NW->getBoundingRectangles();
        rectangles.insert(rectangles.end(), nwRectangles.begin(), nwRectangles.end());
    }
    if (NE != nullptr) {
        std::vector<sf::RectangleShape> neRectangles = NE->getBoundingRectangles();
        rectangles.insert(rectangles.end(), neRectangles.begin(), neRectangles.end());
    }
    if (SW != nullptr) {
        std::vector<sf::RectangleShape> swRectangles = SW->getBoundingRectangles();
        rectangles.insert(rectangles.end(), swRectangles.begin(), swRectangles.end());
    }
    if (SE != nullptr) {
        std::vector<sf::RectangleShape> seRectangles = SE->getBoundingRectangles();
        rectangles.insert(rectangles.end(), seRectangles.begin(), seRectangles.end());
    }

    return rectangles;
}
