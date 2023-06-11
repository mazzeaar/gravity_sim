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
    case BodyType::LISA:
        add_lisa(num_bodies, mass);
        break;
    default:
        std::cout << "Error: Invalid body type" << std::endl;
        break;
    }
}

void ParticleManager::get_particle_area(Vec2& top_left, Vec2& bottom_right)
{
    // initialize the rectangle to the opposite corners of the screen
    top_left = Vec2(width, height);
    bottom_right = Vec2(0, 0);

    // find the bounding square that contains all particles
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
    double armCount = 3.0;       // Number of spiral arms
    double armTightness = 0.5;   // Tightness of the spiral arms
    double armVelocity = 0.03;   // Orbital velocity of the arms

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

    double cubeSize = 800.0; // Size of the cubes
    double speed = 0.04;     // Orbital speed of the cubes

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
}

void ParticleManager::add_random(unsigned count, double mass)
{
    for ( unsigned i = 0; i < count; ++i )
    {
        bodies->mass[i] = mass;
        bodies->radius[i] = std::pow(bodies->mass[i], 1.0 / 3.0);

        double x = rand() % width;
        double y = rand() % height;

        bodies->pos[i] = Vec2(x, y);

        bodies->vel[i] = Vec2(0.0, 0.0);
    }
}

void ParticleManager::add_lisa(unsigned count, double mass)
{
    std::cout << "not implemented yet:)" << std::endl;
}