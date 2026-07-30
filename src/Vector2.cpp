#include "Vector2.h"

namespace CPURenderer {
    double Vector2::GetLength()
    {
        double xSquared = x * x;
        double ySquared = y * y;

        return std::sqrt(xSquared + ySquared);
    }

    Vector2 Vector2::GetNormalized()
    {
        double length = GetLength();

        if (length <= 0.001) {
            CPURenderer::Log("Length of a vector is too small skipping normalization");
            return *this;
        }

        return Vector2(x / length, y / length);
    }

    Vector2 Vector2::operator-(const Vector2 other)
    {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 Vector2::operator*(const float value)
    {
        return Vector2(x * value, y * value);
    }

    void Vector2::Print()
    {
        CPURenderer::Log("Vector -> x: {}, y: {}", x, y);
    }
}
