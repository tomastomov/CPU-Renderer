#include "Vector3.h"

namespace CPURenderer {
	Vector3 CPURenderer::Vector3::operator-(Vector3& other)
	{
		return { x - other.x, y - other.y, z - other.z };
	}
}