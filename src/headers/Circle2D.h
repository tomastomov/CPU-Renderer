#pragma once
#include <Vector2.h>

namespace CPURenderer {
	struct Circle2D {
		Vector2 center;
		Vector2 size;

		static Circle2D Create(Vector2 center, Vector2 size) {
			return Circle2D(center, size);
		}
	};
}