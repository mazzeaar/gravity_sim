#include "Vec2.h"
#include <cmath>

Vec2::Vec2() : x(0.0), y(0.0) {}

Vec2::Vec2(double x, double y) : x(x), y(y) {}

Vec2::Vec2(const Vec2& other) : x(other.x), y(other.y) {}

double Vec2::get_x() const { return x; }
double Vec2::get_y() const { return y; }

void Vec2::set_x(double x) { this->x = x; }
void Vec2::set_y(double y) { this->y = y; }

Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}

Vec2 Vec2::operator*(double scalar) const
{
    return Vec2(x * scalar, y * scalar);
}

Vec2 Vec2::operator/(double scalar) const
{
    return Vec2(x / scalar, y / scalar);
}

Vec2& Vec2::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vec2& Vec2::operator*=(double scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2& Vec2::operator/=(double scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

bool Vec2::operator==(const Vec2& other) const
{
    return x == other.x && y == other.y;
}

bool Vec2::operator!=(const Vec2& other) const
{
    return !(*this == other);
}

double Vec2::dist(const Vec2& other) const
{
    return (*this - other).length();
}

double Vec2::dot(const Vec2& other) const
{
    return x * other.x + y * other.y;
}

Vec2 Vec2::cross(const Vec2& other) const
{
    return Vec2(x * other.y - y * other.x, x * other.y - y * other.x);
}

double Vec2::length() const
{
    return std::hypot(x, y);
}

double Vec2::length_squared() const
{
    return x * x + y * y;
}

Vec2 Vec2::normalize() const
{
    double len = length();
    return Vec2(x / len, y / len);
}

Vec2 Vec2::rotate(double angle) const
{
    double c = std::cos(angle);
    double s = std::sin(angle);
    return Vec2(x * c - y * s, x * s + y * c);
}

std::ostream& operator<<(std::ostream& os, const Vec2& vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}
