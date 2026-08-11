#pragma once

#include <Vertex2.h>
#include <Vector2.h>

namespace CPURenderer {
	struct Triangle2D {
		Vertex2 a;
		Vertex2 b;
		Vertex2 c;
		Vector2 pos;
		Vector2 size;
		float rotate;

		static Triangle2D Create(Vertex2 a, Vertex2 b, Vertex2 c, Vector2 pos, Vector2 size, float rotate) {
			return Triangle2D(a, b, c, pos, size, rotate);
		}
	};
}