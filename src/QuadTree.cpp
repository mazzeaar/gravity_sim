#include "QuadTree.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

QuadTree::QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right, std::shared_ptr<sf::VertexArray> rectangles, bool root) :
    top_left(top_left), bottom_right(bottom_right), rectangles(rectangles)
{
    this->bodies = bodies;

    center_of_mass = Vec2(0, 0);
    mass = 0.0;

    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;

    if ( root )
    {
        for ( unsigned i = 0; i < bodies->get_size(); ++i )
        {
            insert(i);
        }

        if ( rectangles == nullptr )
        {
            rectangles = std::make_shared<sf::VertexArray>(sf::Lines, 8);
        }

        add_root_bounds();
        add_subdivision_bounds();
    }
}

QuadTree::QuadTree(std::shared_ptr<Bodies> bodies, double xmin, double ymin, double xmax, double ymax, std::shared_ptr<sf::VertexArray> rectangles, bool root) :
    QuadTree(bodies, Vec2(xmin, ymin), Vec2(xmax, ymax), rectangles, root)
{}

QuadTree::~QuadTree()
{
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
}


/*----------------------------------------
|             public methods             |
-----------------------------------------*/

void QuadTree::update(double theta, double G, double dt, unsigned long& calculations_per_frame)
{
    unsigned num_threads = std::thread::hardware_concurrency();

    unsigned bodies_size = bodies->get_size();
    unsigned bodies_per_thread = (bodies_size + num_threads - 1) / num_threads;

    std::vector<std::future<unsigned long>> futures(num_threads);

    unsigned start = 0;
    for ( unsigned t = 0; t < num_threads; ++t )
    {
        unsigned end = std::min(start + bodies_per_thread, bodies_size);
        futures[t] = std::async(std::launch::async, [this, theta, G, calculations_per_frame, start, end]()
            {
                unsigned long local_calculations = 0;
                for ( unsigned j = start; j < end; ++j )
                {
                    bodies->acc[j] = Vec2(0, 0);
                    compute_force(j, theta, G, local_calculations);
                }
                return local_calculations;
            });
        start = end;
    }

    unsigned long total_calculations = 0;
    for ( auto& future : futures )
    {
        total_calculations += future.get();
    }
    calculations_per_frame = total_calculations;

    bodies->remove_merged_bodies();
}


/*----------------------------------------
|             private methods            |
-----------------------------------------*/

bool QuadTree::contains(unsigned index) const
{
    return bodies->pos[index].x >= top_left.x
        && bodies->pos[index].x <= bottom_right.x
        && bodies->pos[index].y >= top_left.y
        && bodies->pos[index].y <= bottom_right.y;
}

bool QuadTree::subdivide()
{
    add_subdivision_bounds();

    Vec2 center = (this->top_left + this->bottom_right) / 2.0;

    this->NW = std::make_unique<QuadTree>(bodies, top_left, center, rectangles, false);
    this->NE = std::make_unique<QuadTree>(bodies, Vec2(center.x, top_left.y), Vec2(bottom_right.x, center.y), rectangles, false);
    this->SW = std::make_unique<QuadTree>(bodies, Vec2(top_left.x, center.y), Vec2(center.x, bottom_right.y), rectangles, false);
    this->SE = std::make_unique<QuadTree>(bodies, center, bottom_right, rectangles, false);

    return true;
}

void QuadTree::insert(unsigned index)
{
    QuadTree* current = this;
    unsigned idx = index;

    std::stack<std::pair<QuadTree*, unsigned>> stack;
    stack.push({ current, index });

    Vec2 adjusted_pos = bodies->pos[index] * bodies->mass[index];

    while ( !stack.empty() )
    {
        current = stack.top().first;
        idx = stack.top().second;
        stack.pop();

        while ( !current->is_leaf() )
        {
            current->center_of_mass = ((current->center_of_mass * current->mass) + adjusted_pos) / (current->mass + bodies->mass[idx]);
            current->mass += bodies->mass[idx];

            current = current->get_child_quadrant(idx);

            if ( current == nullptr )
            {
                throw std::runtime_error("Error: Body not contained in any quadrant, this should never happen lol");
            }
        }

        if ( current->body_index == -1 )
        {
            current->center_of_mass = bodies->pos[idx];
            current->mass = bodies->mass[idx];
            current->body_index = idx;
            return;
        }

        else if ( current->subdivide() )
        {
            stack.push({ current->get_child_quadrant(current->body_index), current->body_index });
            stack.push({ current->get_child_quadrant(idx), idx });
            current->body_index = -1;
        }
    }
}

QuadTree* QuadTree::get_child_quadrant(unsigned index)
{
    if ( this->NW->contains(index) )
    {
        return this->NW.get();
    }
    else if ( this->NE->contains(index) )
    {
        return this->NE.get();
    }
    else if ( this->SW->contains(index) )
    {
        return this->SW.get();
    }
    else if ( this->SE->contains(index) )
    {
        return this->SE.get();
    }
    else
    {
        throw std::runtime_error("Error: Body not contained in any quadrant, this should never happen lol");
    }
}

void QuadTree::compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame)
{
    double theta_squared = theta * theta;

    std::stack<QuadTree*> stack;
    QuadTree* current = this;
    stack.push(this);

    while ( !stack.empty() )
    {
        current = stack.top();
        stack.pop();

        if ( current->mass == 0 || current->body_index == index )
        {
            continue;
        }

        const Vec2 direction = current->center_of_mass - bodies->pos[index];
        const double squared_distance = direction.squared_length();

        if ( squared_distance == 0 )
        {
            continue;
        }

        const double squared_size = (current->bottom_right - current->top_left).squared_length();
        const double size_ratio = squared_size / squared_distance;

        if ( size_ratio < theta_squared || current->is_leaf() )
        {
            ++calculations_per_frame;
            double force = calculate_gravitational_force(G, current->mass, bodies->mass[index], squared_distance);
            bodies->add_force(index, direction * force);
        }
        else
        {
            if ( current->NE != nullptr )
            {
                stack.push(current->NE.get());
                stack.push(current->NW.get());
                stack.push(current->SE.get());
                stack.push(current->SW.get());
            }
        }
    }
}


double QuadTree::calculate_gravitational_force(double G, double mass1, double mass2, double squared_distance) const
{
    const double epsilon_squared = 2.0;    // softening factor, else force goes BRRRRRT
    return G * mass1 * mass2 / (squared_distance + epsilon_squared);
}


/*----------------------------------------
|              bound methods             |
-----------------------------------------*/

void QuadTree::add_subdivision_bounds()
{
    sf::Vertex vertex;
    vertex.color = color;

    vertex.position = sf::Vector2f((bottom_right.x + top_left.x) / 2.0f, top_left.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f((bottom_right.x + top_left.x) / 2.0f, bottom_right.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, (bottom_right.y + top_left.y) / 2.0f);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, (bottom_right.y + top_left.y) / 2.0f);
    rectangles->append(vertex);
}

void QuadTree::add_root_bounds()
{
    sf::Vertex vertex;
    vertex.color = color;

    vertex.position = sf::Vector2f(top_left.x, top_left.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, top_left.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, top_left.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, bottom_right.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, bottom_right.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, bottom_right.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, bottom_right.y);
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, top_left.y);
    rectangles->append(vertex);
}