#pragma once

namespace CPURenderer {
	struct Vector3 {
		float x, y, z;

		Vector3 operator-(Vector3& other);
	};
}