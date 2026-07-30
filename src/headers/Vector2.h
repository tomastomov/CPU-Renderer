#pragma once
#include <Log.h>

struct Vector2 {
    float x;
    float y;

    double GetLength();

    Vector2 GetNormalized();

    Vector2 operator-(const Vector2 other);

    Vector2 operator*(const float value);
};