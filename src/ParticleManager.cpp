#include "ParticleManager.h"

/*----------------------------------------
|         Constructor/Destructor         |
-----------------------------------------*/

ParticleManager::ParticleManager(std::shared_ptr<Bodies> bodies, unsigned width, unsigned height) :
    bodies(bodies), width(width), height(height)
{}

ParticleManager::~ParticleManager()
{}

/*----------------------------------------
|             public methods             |
-----------------------------------------*/

void ParticleManager::add_bodies(BodyType type, unsigned num_bodies, double mass)
{
    this->body_type = type;
    this->mass = mass;

    if ( num_bodies > bodies->get_size() )
    {
        bodies->resize(num_bodies);
    }

    switch ( type )
    {
    case BodyType::SPINNING_CIRCLE:
        add_spinning_circle(num_bodies, mass);
        break;
    case BodyType::GALAXY:
        add_galaxy(num_bodies, mass);
        break;
    case BodyType::ROTATING_CUBES:
        add_rotating_cubes(num_bodies, mass);
        break;
    case BodyType::RANDOM:
        add_random(num_bodies, mass);
        break;
    case BodyType::LARGE_CUBE:
        add_large_cube(num_bodies, mass);
        break;
    case BodyType::CUSTOM_SHAPE1:
        add_custom_shape1(num_bodies, mass);
        break;
    default:
        std::cout << "Error: Invalid body type" << std::endl;
        break;
    }
}

void ParticleManager::get_particle_area(Vec2& top_left, Vec2& bottom_right)
{
    // Initialize the rectangle to the opposite corners of the screen
    top_left = Vec2(width, height);
    bottom_right = Vec2(0, 0);

    // Find the bounding square that contains all particles
    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        if ( bodies->pos[i].x < top_left.x )
        {
            top_left.x = bodies->pos[i].x;
        }

        if ( bodies->pos[i].x > bottom_right.x )
        {
            bottom_right.x = bodies->pos[i].x;
        }

        if ( bodies->pos[i].y < top_left.y )
        {
            top_left.y = bodies->pos[i].y;
        }

        if ( bodies->pos[i].y > bottom_right.y )
        {
            bottom_right.y = bodies->pos[i].y;
        }
    }

    // Calculate the width and height
    double width = bottom_right.x - top_left.x;
    double height = bottom_right.y - top_left.y;

    double offset = std::abs(width - height) / 2.0;

    if ( width > height )
    {
        bottom_right.y += offset;
        top_left.y -= offset;
    }
    else
    {
        bottom_right.x += offset;
        top_left.x -= offset;
    }
}

void ParticleManager::reset()
{
    unsigned size = this->bodies->get_size();
    this->bodies->clear();

    add_bodies(this->body_type, size, this->mass);
}

/*----------------------------------------
|            private methods             |
-----------------------------------------*/

void ParticleManager::add_spinning_circle(unsigned count, double mass)
{
    for ( unsigned i = 0; i < count; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = std::pow(bodies->mass[i], 1.0 / 3.0);

        // circle that fits in the screen using the golden ratio
        double x = width / 2.0 + height / 2.0 * 0.418 * cos(2.0 * M_PI * i / count);
        double y = height / 2.0 + height / 2.0 * 0.418 * sin(2.0 * M_PI * i / count);

        bodies->pos[i] = Vec2(x, y);

        // bodies should rotate around the center of the screen, velocity is proportional to the distance to the center
        Vec2 center(width / 2.0, height / 2.0);
        Vec2 direction = (bodies->pos[i] - center).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(center) * 0.06;
    }
}

void ParticleManager::add_galaxy(unsigned count, double mass)
{
    double armCount = 5.0;       // Number of spiral arms
    double armTightness = -0.1;   // Tightness of the spiral arms
    double armVelocity = -0.03;   // Orbital velocity of the arms

    double center_x = width / 2.0;
    double center_y = height / 2.0;
    double max_distance = std::min(center_x, center_y) * 0.9; // Limit the maximum distance from the center

    for ( unsigned i = 0; i < count; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = std::pow(bodies->mass[i], 1.0 / 3.0);

        double angle = static_cast<double>(rand()) / RAND_MAX * 2.0 * M_PI; // Random angle in radians
        double distance = static_cast<double>(rand()) / RAND_MAX * max_distance; // Random distance from the center

        double totalArmAngle = 5.0;

        for ( int j = 0; j < armCount; ++j )
        {
            // Add spiral arm effect to the angle
            double armAngle = armTightness * angle + (j + 1) * distance / max_distance * 2.0 * M_PI;
            double x = center_x + distance * cos(armAngle + totalArmAngle);
            double y = center_y + distance * sin(armAngle + totalArmAngle);

            bodies->pos[i] = Vec2(x, y);

            Vec2 direction = (bodies->pos[i] - Vec2(center_x, center_y)).normalize();
            Vec2 perpendicular = Vec2(-direction.y, direction.x);

            bodies->vel[i] = perpendicular * distance * armVelocity;

            totalArmAngle += armAngle;
        }
    }
}

void ParticleManager::add_rotating_cubes(unsigned count, double mass)
{
    double center_x = width / 2.0;
    double center_y = height / 2.0;

    double center_top_left_x = width / 4.0;
    double center_top_left_y = height / 4.0;

    double center_bottom_right_x = width * 3.0 / 4.0;
    double center_bottom_right_y = height * 3.0 / 4.0;

    double cubeSize = 800.0;
    double speed = 0.02;

    for ( unsigned i = 0; i < bodies->get_size() / 2; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;

        double x = center_top_left_x + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        double y = center_top_left_y + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        bodies->pos[i] = Vec2(x, y);

        Vec2 direction = (bodies->pos[i] - Vec2(center_top_left_x, center_top_left_y)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(Vec2(center_top_left_x, center_top_left_y)) * speed;
    }

    for ( unsigned i = bodies->get_size() / 2; i < bodies->get_size(); ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = cubeSize;

        double x = center_bottom_right_x + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        double y = center_bottom_right_y + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        bodies->pos[i] = Vec2(x, y);

        Vec2 direction = (bodies->pos[i] - Vec2(center_bottom_right_x, center_bottom_right_y)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(Vec2(center_bottom_right_x, center_bottom_right_y)) * speed;
    }

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        Vec2 direction = (bodies->pos[i] - Vec2(center_x, center_y)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] += perpendicular * bodies->pos[i].dist(Vec2(center_x, center_y)) * 0.03;
    }
}

void ParticleManager::add_random(unsigned count, double mass)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(0.0, 1.0);

    double edgeOffsetX = width / 10.0;
    double edgeOffsetY = height / 10.0;

    for ( unsigned i = 0; i < count; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;

        // Generate random positions using Gaussian distribution
        double x = width / 2.0 + edgeOffsetX * distribution(gen);
        double y = height / 2.0 + edgeOffsetY * distribution(gen);
        bodies->pos[i] = Vec2(x, y);

        double velocity_scalar = 5.0;

        Vec2 direction = (bodies->pos[i] - Vec2(width / 2.0, height / 2.0)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        double vx = perpendicular.x * velocity_scalar;
        double vy = perpendicular.y * velocity_scalar;

        bodies->vel[i] = Vec2(vx, vy);
    }
}

void ParticleManager::add_large_cube(unsigned count, double mass)
{
    double center_x = width / 2.0;
    double center_y = height / 2.0;

    double cubeSize = width;
    double speed = 0.03;

    for ( unsigned i = 0; i < bodies->get_size(); ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;

        double x = center_x + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        double y = center_y + (static_cast<double>(rand()) / RAND_MAX - 0.5) * cubeSize;
        bodies->pos[i] = Vec2(x, y);

        Vec2 direction = (bodies->pos[i] - Vec2(center_x, center_y)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(Vec2(center_x, center_y)) * speed;
    }
}

void ParticleManager::add_custom_shape1(unsigned count, double mass)
{
    double centerX = width / 2.0;
    double centerY = height / 2.0;
    double radius = std::min(centerX, centerY) * 0.8;
    double speed = 0.02;

    double petalCount = 6.0;     // Number of petals

    // Calculate the number of bodies per flower (1/4 of the total count)
    unsigned bodiesPerFlower = count / 8;

    for ( unsigned i = 0; i < count; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = 1.0;

        // Determine the current flower index based on the body index
        unsigned flowerIndex = i / bodiesPerFlower;

        double angle = 2.0 * M_PI * i / bodiesPerFlower;  // Angle between each body

        double petalAngle = angle * petalCount;  // Angle for the petal effect

        // Determine the radius and petal radius based on the flower index
        double currentRadius = radius * (1.0 - 0.1 * flowerIndex);
        double currentPetalRadius = 200.0 * (1.0 + 0.1 * flowerIndex);

        double x = centerX + currentRadius * cos(angle) + currentPetalRadius * cos(petalAngle);
        double y = centerY + currentRadius * sin(angle) + currentPetalRadius * sin(petalAngle);
        bodies->pos[i] = Vec2(x, y);

        Vec2 direction = (bodies->pos[i] - Vec2(centerX, centerY)).normalize();
        Vec2 perpendicular = Vec2(-direction.y, direction.x);

        bodies->vel[i] = perpendicular * bodies->pos[i].dist(Vec2(centerX, centerY)) * speed;
    }
}
