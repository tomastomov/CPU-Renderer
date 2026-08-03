#pragma once

#include <Vector2.h>

struct Triangle2D {
	CPURenderer::Vector2 a;
	CPURenderer::Vector2 b;
	CPURenderer::Vector2 c;
	CPURenderer::Vector2 pos;
	CPURenderer::Vector2 size;
	float rotate;

	static Triangle2D Create(CPURenderer::Vector2 a, CPURenderer::Vector2 b, CPURenderer::Vector2 c, CPURenderer::Vector2 pos, CPURenderer::Vector2 size, float rotate) {
		return Triangle2D(a, b, c, pos, size, rotate);
	}
};