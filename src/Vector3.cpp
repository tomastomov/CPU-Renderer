#include "Vector3.h"

namespace CPURenderer {
	Vector3 CPURenderer::Vector3::operator-(Vector3& other)
	{
		return { x - other.x, y - other.y, z - other.z };
	}
	Vector3 Vector3::operator*(float value)
	{
		return { x * value, y * value, z * value };
	}
}