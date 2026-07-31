#pragma once
#include <Vector2.h>

namespace CPURenderer {
	struct Matrix3x3 {
		float arr[3][3];

		Matrix3x3(const float(&a)[3][3]);

		void Scale(const Vector2 v);

		void Rotate(const float angle);

		void Translate(const Vector2 v);

		static Matrix3x3 GetIdentity();

		static Matrix3x3 GetTranslated(const Vector2 v);

		static Matrix3x3 GetScaled(const Vector2 v);

		static Matrix3x3 GetRotated(float angle);

		void Print();

		Matrix3x3 operator *(const Matrix3x3& other);

		Vector2 operator *(const Vector2 v);
	};
}