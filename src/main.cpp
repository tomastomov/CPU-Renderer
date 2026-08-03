#include "pch.h"
#include <Log.h>
#include <SDL3/SDL.h>
#include <Vector2.h>
#include <Matrix3x3.h>
#include <Quad2D.h>

using CPURenderer::Vector2;
using CPURenderer::Matrix3x3;

//TODO:: figure out on how to know if a point is inside a triangle or not

struct Triangle2D {
	Vector2 a;
	Vector2 b;
	Vector2 c;
	Vector2 pos;
	Vector2 size;
	float rotate;

	static Triangle2D Create(Vector2 a, Vector2 b, Vector2 c, Vector2 pos, Vector2 size, float rotate) {
		return Triangle2D(a, b, c, pos, size, rotate);
	}
};

static uint32_t GetColorFromARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

static void DrawPixel(int x, int y, uint32_t* frameBuffer, const int WIDTH, const int HEIGHT, Vector2 end, Vector2 line) {
	int endX = std::floor(end.x);
	int endY = std::floor(end.y);

	bool isOnLine = (line.x <= 0 ? x >= endX : x <= endX) && (line.y <= 0 ? y >= endY : y <= endY);

	if (!isOnLine) {
		CPURenderer::Log("({}, {}) is not on line ({}, {}) with slope ({}, {})", x, y, endX, endY, line.x, line.y);
	}
	
	if (x < WIDTH && y < HEIGHT && x >= 0 && y >= 0 && isOnLine) {
		int frameBufferIndex = y * WIDTH + x;

		frameBuffer[frameBufferIndex] = GetColorFromARGB(255, 0, 0, 255);
	}
}

static void DrawLine(Vector2 a, Vector2 b, uint32_t* frameBuffer, const int WIDTH, const int HEIGHT) {
	CPURenderer::Log("Processing point x: {}, y: {} and x1: {}, y1: {} ", a.x, a.y, b.x, b.y);
	Vector2 line = b - a;
	Vector2 normalizedLine = line.GetNormalized();

	if (normalizedLine == Vector2::ZERO_VECTOR) {
		return;
	}

	Vector2 start = { a.x, a.y };
	Vector2 end = { b.x, b.y };

	bool xCondition = normalizedLine.x >= 0.0f ? start.x <= end.x : start.x >= end.x;
	bool yCondition = normalizedLine.y >= 0.0f ? start.y <= end.y : start.y >= end.y;

	while (xCondition && yCondition) {
		start.x += normalizedLine.x;
		start.y += normalizedLine.y;

		DrawPixel(std::ceil(start.x), std::ceil(start.y), frameBuffer, WIDTH, HEIGHT, end, normalizedLine);
		DrawPixel(std::floor(start.x), std::floor(start.y), frameBuffer, WIDTH, HEIGHT, end, normalizedLine);
		DrawPixel(std::ceil(start.x), std::floor(start.y), frameBuffer, WIDTH, HEIGHT, end, normalizedLine);
		DrawPixel(std::floor(start.x), std::ceil(start.y), frameBuffer, WIDTH, HEIGHT, end, normalizedLine);

		xCondition = normalizedLine.x >= 0.0f ? start.x <= end.x: start.x >= end.x;
		yCondition = normalizedLine.y >= 0.0f ? start.y <= end.y : start.y >= end.y;
		//CPURenderer::Log("Drawing pixel point x: {}, y: {} at screen location {}", startX, startY, frameBufferIndex);
	}
}

static void DrawTriangle(Triangle2D& triangle, uint32_t* frameBuffer, int WIDTH, int HEIGHT) {
	double abLen = (triangle.b - triangle.a).GetLength();
	double bcLen = (triangle.c - triangle.b).GetLength();
	double caLen = (triangle.a - triangle.c).GetLength();

	if (abLen + bcLen <= caLen ||
		abLen + caLen <= bcLen ||
		bcLen + caLen <= abLen)
	{
		CPURenderer::Log("Invalid triangle");
		return;
	}

	Matrix3x3 transform = Matrix3x3::GetTranslated(triangle.pos) * Matrix3x3::GetRotated(triangle.rotate) * Matrix3x3::GetScaled(triangle.size);

	Vector2 a = triangle.a;
	Vector2 b = triangle.b;
	Vector2 c = triangle.c;

	a = transform * a;
	b = transform * b;
	c = transform * c;

	Vector2 caLine = (c - a).GetNormalized();

	Vector2 cbLine = (c - b).GetNormalized();

	auto hasNotReachedLineEnd = [](Vector2 line, Vector2 a, Vector2 b) -> bool {
		return (line.x >= 0.0f ? a.x <= b.x : a.x >= b.x) && (line.y >= 0.0f ? a.y <= b.y : a.y >= b.y);
	};

	bool keepDrawing = hasNotReachedLineEnd(caLine, a, c) && hasNotReachedLineEnd(cbLine, b, c);

	DrawLine(a, b, frameBuffer, WIDTH, HEIGHT);
	DrawLine(a, c, frameBuffer, WIDTH, HEIGHT);
	DrawLine(b, c, frameBuffer, WIDTH, HEIGHT);

	while (keepDrawing) {
		a.x += caLine.x;
		a.y += caLine.y;

		b.x += cbLine.x;
		b.y += cbLine.y;
			
		DrawLine(a, b, frameBuffer, WIDTH, HEIGHT);

		keepDrawing = hasNotReachedLineEnd(caLine, a, c) && hasNotReachedLineEnd(cbLine, b, c);
	}

	return;
}

static void DrawQuad(Quad2D quad, uint32_t* frameBuffer, const int WIDTH, const int HEIGHT) {
	auto& indeces = quad.indeces;
	if (indeces.size() == 0 || indeces.size() % 3 != 0) {
		CPURenderer::Log("Not enough indeces provided");
		return;
	}

	for (int i = 0; i < indeces.size(); i += 3) {
		Triangle2D triangle = Triangle2D::Create(quad.points[indeces[i]], quad.points[indeces[i + 1]], quad.points[indeces[i + 2]], quad.pos, quad.size, quad.rotate);
		DrawTriangle(triangle, frameBuffer, WIDTH, HEIGHT);
	}
}
	
int main() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		CPURenderer::Log("Failed to init sdl video");
		return -1;
	}

	CPURenderer::Log("SDL init successfully");

	constexpr int WIDTH = 1920;
	constexpr int HEIGHT = 1080;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	if (!SDL_CreateWindowAndRenderer(
		"CPU Renderer",
		WIDTH,
		HEIGHT,
		SDL_WINDOW_RESIZABLE,
		&window,
		&renderer)) {
		CPURenderer::Log("{}", SDL_GetError());
		return -1;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	uint32_t* frameBuffer = new uint32_t[WIDTH * HEIGHT]{};

	Vector2 bottomLeft = { -0.5f, -0.5f };
	Vector2 topLeft = { -0.5f, 0.5f };
	Vector2 bottomRight = { 0.5f, -0.5f };
	Vector2 topRight = { 0.5f, 0.5f };

	Quad2D quad = Quad2D::Create({ 500.0f, 500.0f }, { 800.0f, 800.0f }, 0.0f, { bottomLeft, topLeft, bottomRight, topRight }, { 0, 1, 2, 1, 2, 3});

	DrawQuad(quad, frameBuffer, WIDTH, HEIGHT);

	/*Triangle2D triangle = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { WIDTH * 0.5f, HEIGHT * 0.5f }, { 500.0f, 500.0f }, 10.0f);
	Triangle2D triangle2 = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { 200.0f, 200.0f }, { 10.0f, 10.0f }, 0.0f);

	DrawTriangle(triangle, frameBuffer, WIDTH, HEIGHT);
	DrawTriangle(triangle2, frameBuffer, WIDTH, HEIGHT);*/

	SDL_UpdateTexture(texture, nullptr, frameBuffer, WIDTH * sizeof(uint32_t));

	bool running = true;

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_KEY_DOWN) {
				CPURenderer::Log("Received keydown event");
				if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
					running = false;
				}
			}
			else if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	SDL_Quit();

	delete[] frameBuffer;
	frameBuffer = nullptr;

	return 0;
}
