#pragma once
#include <Vector2.h>
#include <Vertex2.h>

namespace CPURenderer {
	struct Quad2D {
		Vector2 pos;
		Vector2 size;
		float rotate;
		std::vector<Vertex2> points;
		std::vector<uint32_t> indeces;

		static Quad2D Create(Vector2 pos, Vector2 size, float rotate, std::vector<Vertex2>&& points, std::vector<uint32_t>&& indeces) {
			return Quad2D(pos, size, rotate, points, indeces);
		}
	};
}