#ifndef VEC2_H
#define VEC2_H

#include <ostream>

class Vec2 {
public:
    double x, y;

    Vec2(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    Vec2(const Vec2& other) {
        x = other.x;
        y = other.y;
    }

    double get_x() const { return x; }
    double get_y() const { return y; }

    void set_x(double x) { this->x = x; }
    void set_y(double y) { this->y = y; }

    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2& operator-() {
        x = -x;
        y = -y;
        return *this;
    }

    Vec2 operator*(double scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    Vec2 operator/(double scalar) const {
        return Vec2(x / scalar, y / scalar);
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2& other) const {
        return !(*this == other);
    }

    Vec2 dist(const Vec2& other) const {
        return (*this - other).length();
    }

    double dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    Vec2 cross(const Vec2& other) const {
        return x * other.y - y * other.x;
    }

    double length() {
        return std::sqrt(x * x + y * y);
    }

    double length_squared() {
        return x * x + y * y;
    }

    Vec2 normalize() {
        return *this / length();
    }

    Vec2 rotate(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);
        double x_new = x * c - y * s;
        double y_new = x * s + y * c;
        x = x_new;
        y = y_new;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }
};

#endif