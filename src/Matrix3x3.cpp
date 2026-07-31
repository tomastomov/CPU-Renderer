#include "Matrix3x3.h"
#include <Log.h>

namespace CPURenderer {
	Matrix3x3::Matrix3x3(const float(&a)[3][3])
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				arr[i][j] = a[i][j];
			}
		}
	}

	void Matrix3x3::Scale(const Vector2 v)
	{
		arr[0][0] *= v.x;
		arr[1][1] *= v.y;
	}

	void Matrix3x3::Rotate(const float angle)
	{
		static constexpr float epsilon = 1e-6f;
		float radians = angle * std::numbers::pi_v<float> / 180.0f;
		float c = std::cos(radians);
		float s = std::sin(radians);

		if (std::abs(s) < epsilon) {
			s = 0.0f;
		}

		if (std::abs(c) < epsilon) {
			c = 0.0f;
		}

		arr[0][0] = c;
		arr[0][1] = -s;
		arr[1][0] = s;
		arr[1][1] = c;
	}

	void Matrix3x3::Translate(const Vector2 v)
	{
		arr[0][2] = v.x;
		arr[1][2] = v.y;
	}

	Matrix3x3 Matrix3x3::GetIdentity()
	{
		static constexpr float identity[3][3] = {
				{1.0f, 0.0f, 0.0f,}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
		};

		return Matrix3x3(identity);
	}

	Matrix3x3 Matrix3x3::GetTranslated(const Vector2 v)
	{
		Matrix3x3 identity = GetIdentity();
		identity.Translate(v);

		return identity;
	}

	Matrix3x3 Matrix3x3::GetScaled(const Vector2 v)
	{
		Matrix3x3 identity = GetIdentity();
		identity.Scale(v);

		return identity;
	}

	Matrix3x3 Matrix3x3::GetRotated(float angle)
	{
		Matrix3x3 identity = GetIdentity();
		identity.Rotate(angle);

		return identity;
	}

	void Matrix3x3::Print()
	{
		CPURenderer::Log("First row: {} {} {}", arr[0][0], arr[0][1], arr[0][2]);
		CPURenderer::Log("Second row: {} {} {}", arr[1][0], arr[1][1], arr[1][2]);
		CPURenderer::Log("Third row: {} {} {}", arr[2][0], arr[2][1], arr[2][2]);
	}

	Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other)
	{
		float result[3][3];

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				result[i][j] = (arr[i][0] * other.arr[0][j]) + (arr[i][1] * other.arr[1][j]) + (arr[i][2] * other.arr[2][j]);
			}
		}

		return Matrix3x3(result);
	}

	Vector2 Matrix3x3::operator*(const Vector2 v)
	{
		Vector2 res;

		res.x = (arr[0][0] * v.x) + (arr[0][1] * v.y) + (arr[0][2] * 1);
		res.y = (arr[1][0] * v.x) + (arr[1][1] * v.y) + (arr[1][2] * 1);

		return res;
	}

}