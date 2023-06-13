#include "QuadTree.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

QuadTree::QuadTree(std::shared_ptr<Bodies> bodies, Vec2 top_left, Vec2 bottom_right, std::shared_ptr<sf::VertexArray> rectangles, bool root) :
    top_left(top_left), bottom_right(bottom_right), rectangles(rectangles)
{
    this->bodies = bodies;

    depth = 0;

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
            bodies->acc[i] = Vec2(0, 0);
            this->insert(i);
        }
    }

    if ( rectangles == nullptr )
    {
        rectangles = std::make_shared<sf::VertexArray>(sf::Lines, 5);
    }

    sf::Vertex vertex;

    vertex.position = sf::Vector2f(top_left.x, top_left.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, top_left.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, top_left.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, bottom_right.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(bottom_right.x, bottom_right.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, bottom_right.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, bottom_right.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);

    vertex.position = sf::Vector2f(top_left.x, top_left.y);
    vertex.color = sf::Color::Green;
    rectangles->append(vertex);
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
    const unsigned num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads(num_threads);

    const unsigned chunk_size = (bodies->get_size() + num_threads - 1) / num_threads;

    for ( unsigned t = 0; t < num_threads; ++t )
    {
        threads[t] = std::thread([&, t]()
            {
                const unsigned start = t * chunk_size;
                const unsigned end = std::min((t + 1) * chunk_size, bodies->get_size());

                for ( unsigned j = start; j < end; ++j )
                {
                    compute_force(j, theta, G, calculations_per_frame);
                }
            });
    }

    for ( auto& thread : threads )
    {
        thread.join();
    }

    bodies->remove_merged_bodies();
    bodies->update(dt);
}

/*----------------------------------------
|             private methods            |
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
    const double merge_threshold = 0.1;

    if ( bottom_right.x - top_left.x <= merge_threshold || bottom_right.y - top_left.y <= merge_threshold )
    {
        return false;
    }

    this->NW = std::make_unique<QuadTree>(bodies, top_left, (top_left + bottom_right) / 2.0, rectangles, false);
    this->NE = std::make_unique<QuadTree>(bodies, Vec2((top_left.x + bottom_right.x) / 2.0, top_left.y), Vec2(bottom_right.x, (top_left.y + bottom_right.y) / 2.0), rectangles, false);
    this->SW = std::make_unique<QuadTree>(bodies, Vec2(top_left.x, (top_left.y + bottom_right.y) / 2.0), Vec2((top_left.x + bottom_right.x) / 2.0, bottom_right.y), rectangles, false);
    this->SE = std::make_unique<QuadTree>(bodies, (top_left + bottom_right) / 2.0, bottom_right, rectangles, false);

    NW->depth = depth + 1;
    NE->depth = depth + 1;
    SW->depth = depth + 1;
    SE->depth = depth + 1;

    return true;
}

void QuadTree::insert(unsigned index)
{
    if ( this->mass == 0 )
    {
        this->body_index = index;
        this->mass = bodies->mass[index];
        this->center_of_mass = bodies->pos[index];

        return;
    }

    this->center_of_mass = (this->center_of_mass * this->mass + bodies->pos[index] * bodies->mass[index]) / (this->mass + bodies->mass[index]);
    this->mass += bodies->mass[index];

    if ( !this->is_leaf() )
    {
        if ( this->NW->contains(index) ) this->NW->insert(index);
        if ( this->NE->contains(index) ) this->NE->insert(index);
        if ( this->SW->contains(index) ) this->SW->insert(index);
        if ( this->SE->contains(index) ) this->SE->insert(index);

        return;
    }
    else if ( this->body_index == -1 )
    {
        this->body_index = index;
        return;
    }
    else
    {
        if ( this->subdivide() )
        {
            unsigned old_index = this->body_index;
            this->body_index = -1;

            this->insert(old_index);
            this->insert(index);
            return;
        }
        else
        {
            return;
        }
    }
}

void QuadTree::compute_force(unsigned index, double theta, double G, unsigned long& calculations_per_frame)
{
    std::stack<QuadTree*> stack;
    stack.push(this);

    while ( !stack.empty() )
    {
        QuadTree* node = stack.top();
        stack.pop();

        if ( node->mass == 0 || node->body_index == index )
        {
            continue;
        }

        Vec2 direction = node->center_of_mass - bodies->pos[index];
        double squared_distance = direction.squared_length();
        double squared_size = (node->bottom_right - node->top_left).squared_length();

        if ( squared_size / squared_distance < theta || node->is_leaf() )
        {
            ++calculations_per_frame;
            double force = calculate_gravitational_force(G, node->mass, bodies->mass[index], squared_distance);
            bodies->add_force(index, direction * force);
        }
        else
        {
            if ( node->NE != nullptr ) stack.push(node->NE.get());
            if ( node->NW != nullptr ) stack.push(node->NW.get());
            if ( node->SE != nullptr ) stack.push(node->SE.get());
            if ( node->SW != nullptr ) stack.push(node->SW.get());
        }
    }
}

double QuadTree::calculate_gravitational_force(double G, double mass1, double mass2, double squared_distance)
{
    // softening factor, else force goes BRRRRRT
    const double epsilon_squared = 2.0;
    return G * mass1 * mass2 / (squared_distance + epsilon_squared);
}
