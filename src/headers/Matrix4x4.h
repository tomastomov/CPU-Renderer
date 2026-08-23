#pragma once
#include <Vector3.h>

namespace CPURenderer {
	struct Matrix4x4 {
		float arr[4][4];

		Matrix4x4(const float(&a)[4][4]);

		void Scale(const Vector3 v);

		void Rotate(const float angle);

		void Translate(const Vector3 v);

		static Matrix4x4 GetIdentity();

		static Matrix4x4 GetTranslated(const Vector3 v);

		static Matrix4x4 GetScaled(const Vector3 v);

		static Matrix4x4 GetRotated(float angle);

		void Print();

		Matrix4x4 operator *(const Matrix4x4& other);

		Vector3 operator *(const Vector3 v);
	};
}
