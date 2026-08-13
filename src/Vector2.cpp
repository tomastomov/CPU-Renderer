#include "Vector2.h"

namespace CPURenderer {
    const Vector2 Vector2::ZERO_VECTOR = { 0.0f, 0.0f };

    double Vector2::GetLength()
    {
        double xSquared = x * x;
        double ySquared = y * y;

        return std::sqrt(xSquared + ySquared);
    }

    double Vector2::GetDistance(Vector2 other)
    {
        double xDelta = x - other.x;
        double yDelta = y - other.y;

        double xSquared = xDelta * xDelta;
        double ySquared = yDelta * yDelta;

        return std::sqrt(xSquared + ySquared);
    }

    Vector2 Vector2::GetNormalized()
    {
        double length = GetLength();

        if (length <= 0.0001) {
            CPURenderer::Log("Length of a vector is too small skipping normalization, {}", length);
            return ZERO_VECTOR;
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

    float Vector2::Cross(Vector2 other)
    {
        return x * other.y - y * other.x;
    }

    bool Vector2::operator==(const Vector2 other)
    {
        return x == other.x && y == other.y;
    }

    void Vector2::Print()
    {
        CPURenderer::Log("Vector -> x: {}, y: {}", x, y);
    }
}
