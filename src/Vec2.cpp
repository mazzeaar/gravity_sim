#include "Vec2.h"
#include <cmath>

Vec2::Vec2() : x(0.0), y(0.0) {}

Vec2::Vec2(double x, double y) : x(x), y(y) {}

Vec2::Vec2(const Vec2& other) : x(other.x), y(other.y) {}

std::ostream& operator<<(std::ostream& os, const Vec2& vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}
