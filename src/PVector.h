//
// Created by Cengiz Cimen on 1/04/2023.
//

#ifndef PROCEDURALANIMATION_PVECTOR_H
#define PROCEDURALANIMATION_PVECTOR_H


#include <cmath>

class PVector {
public:
    double x, y, z;

    PVector() : x(0.0), y(0.0) {}

    PVector(double _x, double _y, double _z = 0) : x(_x), y(_y), z(_z) {}

    PVector operator+(const PVector& other) const {
        return PVector(x + other.x, y + other.y);
    }

    PVector operator-(const PVector& other) const {
        return PVector(x - other.x, y - other.y);
    }

    PVector operator*(double scalar) const {
        return PVector(x * scalar, y * scalar);
    }

    PVector operator/(double scalar) const {
        return PVector(x / scalar, y / scalar);
    }

    PVector& operator+=(const PVector& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    PVector& operator-=(const PVector& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    PVector& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    PVector& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    double length() const {
        return std::sqrt(x * x + y * y);
    }

    double lengthSquared() const {
        return x * x + y * y;
    }

    PVector normalized() const {
        double len = length();
        if (len == 0.0) {
            return PVector();
        } else {
            return PVector(x / len, y / len);
        }
    }

    double dot(const PVector& other) const {
        return x * other.x + y * other.y;
    }

};

#endif //PROCEDURALANIMATION_PVECTOR_H
