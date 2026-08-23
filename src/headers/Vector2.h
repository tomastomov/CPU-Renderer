#pragma once
#include <Log.h>

namespace CPURenderer {
    struct Vector2 {
        float x;
        float y;

        static const Vector2 ZERO_VECTOR;

        double GetLength();

        double GetLengthSquared();

        double GetDistance(Vector2 other);

        Vector2 GetNormalized();

        Vector2 operator-(const Vector2 other);

        Vector2 operator*(const float value);

        float Cross(Vector2 other);

        bool operator==(const Vector2 other);

        void Print();
    };
}