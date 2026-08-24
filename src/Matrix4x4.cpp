#include <Matrix4x4.h>
#include <Log.h>

namespace CPURenderer {
	Matrix4x4::Matrix4x4(const float(&a)[4][4])
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				arr[i][j] = a[i][j];
			}
		}
	}

	inline void Matrix4x4::Scale(const Vector3 v)
	{
		arr[0][0] *= v.x;
		arr[1][1] *= v.y;
		arr[2][2] *= v.z;
	}

	inline void Matrix4x4::RotateAroundY(const float angle)
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
		arr[0][2] = -s;
		arr[2][0] = s;
		arr[2][2] = c;
	}

	inline void Matrix4x4::RotateAroundZ(const float angle)
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

	inline void Matrix4x4::RotateAroundX(const float angle)
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

		arr[1][1] = c;
		arr[1][2] = -s;
		arr[2][1] = s;
		arr[2][2] = c;
	}

	inline void Matrix4x4::Translate(const Vector3 v)
	{
		arr[0][3] = v.x;
		arr[1][3] = v.y;
		arr[2][3] = v.z;
	}

	Matrix4x4 Matrix4x4::GetIdentity()
	{
		static constexpr float identity[4][4] = {
			{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}
		};

		return Matrix4x4(identity);
	}

	Matrix4x4 Matrix4x4::GetTranslated(const Vector3 v)
	{
		Matrix4x4 identity = GetIdentity();
		identity.Translate(v);

		return identity;
	}

	Matrix4x4 Matrix4x4::GetScaled(const Vector3 v)
	{
		Matrix4x4 identity = GetIdentity();
		identity.Scale(v);

		return identity;
	}

	Matrix4x4 Matrix4x4::GetRotatedAroundZ(float angle)
	{
		Matrix4x4 identity = GetIdentity();
		identity.RotateAroundZ(angle);

		return identity;
	}

	Matrix4x4 Matrix4x4::GetRotatedAroundY(float angle)
	{
		Matrix4x4 identity = GetIdentity();
		identity.RotateAroundY(angle);

		return identity;
	}

	Matrix4x4 Matrix4x4::GetRotatedAroundX(float angle)
	{
		Matrix4x4 identity = GetIdentity();
		identity.RotateAroundX(angle);

		return identity;
	}

	inline void Matrix4x4::Print()
	{
		CPURenderer::Log("First row: {} {} {} {}", arr[0][0], arr[0][1], arr[0][2], arr[0][3]);
		CPURenderer::Log("Second row: {} {} {} {}", arr[1][0], arr[1][1], arr[1][2], arr[1][3]);
		CPURenderer::Log("Third row: {} {} {} {}", arr[2][0], arr[2][1], arr[2][2], arr[2][3]);
		CPURenderer::Log("Fourth row: {} {} {} {}", arr[3][0], arr[3][1], arr[3][2], arr[3][3]);
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other)
	{
		float result[4][4];

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result[i][j] = (arr[i][0] * other.arr[0][j]) + (arr[i][1] * other.arr[1][j]) + (arr[i][2] * other.arr[2][j] + (arr[i][3] * other.arr[3][j]));
			}
		}

		return Matrix4x4(result);
	}

	Vector3 Matrix4x4::operator*(const Vector3 v)
	{
		Vector3 res;

		res.x = (arr[0][0] * v.x) + (arr[0][1] * v.y) + (arr[0][2] * v.z) + (arr[0][3] * 1.0f);
		res.y = (arr[1][0] * v.x) + (arr[1][1] * v.y) + (arr[1][2] * v.z) + (arr[1][3] * 1.0f);
		res.z = (arr[2][0] * v.x) + (arr[2][1] * v.y) + (arr[2][2] * v.z) + (arr[2][3] * 1.0f);

		return res;
	}
}