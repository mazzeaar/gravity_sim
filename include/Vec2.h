#ifndef VEC2_H
#define VEC2_H

#include <ostream>
#include <cmath>
#include <iostream> 

class Vec2 {
public:
    double x, y;

    Vec2() : x(0.0), y(0.0) {}

    Vec2(double x, double y) : x(x), y(y) {}

    Vec2(const Vec2& other) : x(other.x), y(other.y) {}

    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }

    inline double get_x() const { return x; }
    inline double get_y() const { return y; }

    inline void set_x(double x) { this->x = x; }
    inline void set_y(double y) { this->y = y; }

    inline Vec2 operator+(const Vec2& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    inline Vec2 operator-(const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    inline Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    inline Vec2 operator*(double scalar) const
    {
        return Vec2(x * scalar, y * scalar);
    }

    inline Vec2 operator/(double scalar) const
    {
        return Vec2(x / scalar, y / scalar);
    }

    inline Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline Vec2& operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    inline Vec2& operator/=(double scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    inline bool operator==(const Vec2& other) const
    {
        return x == other.x && y == other.y;
    }

    inline bool operator!=(const Vec2& other) const
    {
        return !(*this == other);
    }

    inline double dist(const Vec2& other) const
    {
        return (*this - other).length();
    }

    inline double dot(const Vec2& other) const
    {
        return x * other.x + y * other.y;
    }

    inline Vec2 cross(const Vec2& other) const
    {
        return Vec2(x * other.y - y * other.x, x * other.y - y * other.x);
    }

    inline double length() const
    {
        return std::hypot(x, y);
    }

    inline double squared_length() const
    {
        return x * x + y * y;
    }

    inline Vec2 normalize() const
    {
        double len = length();
        return Vec2(x / len, y / len);
    }

    inline Vec2 rotate(double angle) const
    {
        double c = std::cos(angle);
        double s = std::sin(angle);
        return Vec2(x * c - y * s, x * s + y * c);
    }
};

#endif
