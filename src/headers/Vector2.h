#pragma once
#include <Log.h>

namespace CPURenderer {
    struct Vector2 {
        float x;
        float y;

        static const Vector2 ZERO_VECTOR;

        double GetLength();

        Vector2 GetNormalized();

        Vector2 operator-(const Vector2 other);

        Vector2 operator*(const float value);

        bool operator==(const Vector2 other);

        void Print();
    };
}