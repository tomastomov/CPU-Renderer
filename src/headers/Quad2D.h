#pragma once
#include <Vector2.h>s

struct Quad2D {
	CPURenderer::Vector2 pos;
	CPURenderer::Vector2 size;
	float rotate;
	std::vector<CPURenderer::Vector2> points;
	std::vector<uint32_t> indeces;

	static Quad2D Create(CPURenderer::Vector2 pos, CPURenderer::Vector2 size, float rotate, std::vector<CPURenderer::Vector2>&& points, std::vector<uint32_t>&& indeces) {
		return Quad2D(pos, size, rotate, points, indeces);
	}
};