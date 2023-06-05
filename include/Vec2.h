#ifndef VEC2_H
#define VEC2_H

#include <ostream>

class Vec2 {
public:
    double x, y;

    Vec2();
    Vec2(double x, double y);
    Vec2(const Vec2& other);

    double get_x() const;
    double get_y() const;

    void set_x(double x);
    void set_y(double y);

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator-() const;
    Vec2 operator*(double scalar) const;
    Vec2 operator/(double scalar) const;

    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator*=(double scalar);
    Vec2& operator/=(double scalar);

    bool operator==(const Vec2& other) const;
    bool operator!=(const Vec2& other) const;

    double dist(const Vec2& other) const;
    double dot(const Vec2& other) const;
    Vec2 cross(const Vec2& other) const;

    double length() const;
    double length_squared() const;
    Vec2 normalize() const;
    Vec2 rotate(double angle) const;

    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec);
};

#endif
