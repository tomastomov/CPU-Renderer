#include "pch.h"
#include <Log.h>
#include <SDL3/SDL.h>
#include <Vector2.h>
#include <Matrix3x3.h>
#include <Quad2D.h>
#include <Triangle2D.h>
#include <GameConfig.h>

using CPURenderer::Vector2;
using CPURenderer::Matrix3x3;
using CPURenderer::GameConfig;

//TODO:: figure out on how to know if a point is inside a triangle or not
//TODO:: check if it is inside viewport

static uint32_t GetColorFromARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

struct Circle2D {
	Vector2 center;
	Vector2 size;

	static Circle2D Create(Vector2 center, Vector2 size) {
		return Circle2D(center, size);
	}
};

static void DrawPixel(int x, int y, uint32_t* frameBuffer, const GameConfig& config, Vector2 end, Vector2 line) {
	int endX = std::floor(end.x);
	int endY = std::floor(end.y);

	bool isOnLine = (line.x <= 0 ? x >= endX : x <= endX) && (line.y <= 0 ? y >= endY : y <= endY);

	if (!isOnLine) {
		//CPURenderer::Log("({}, {}) is not on line ({}, {}) with slope ({}, {})", x, y, endX, endY, line.x, line.y);
	}
	
	if (x < config.WIDTH && y < config.HEIGHT && x >= 0 && y >= 0 && isOnLine) {
		int frameBufferIndex = y * config.WIDTH + x;

		frameBuffer[frameBufferIndex] = GetColorFromARGB(255, 0, 0, 255);
	}
}

static void DrawLine(Vector2 a, Vector2 b, uint32_t* frameBuffer, const GameConfig& config) {
	//CPURenderer::Log("Processing point x: {}, y: {} and x1: {}, y1: {} ", a.x, a.y, b.x, b.y);
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

		DrawPixel(std::ceil(start.x), std::ceil(start.y), frameBuffer, config, end, normalizedLine);
		DrawPixel(std::floor(start.x), std::floor(start.y), frameBuffer, config, end, normalizedLine);
		DrawPixel(std::ceil(start.x), std::floor(start.y), frameBuffer, config, end, normalizedLine);
		DrawPixel(std::floor(start.x), std::ceil(start.y), frameBuffer, config, end, normalizedLine);

		xCondition = normalizedLine.x >= 0.0f ? start.x <= end.x: start.x >= end.x;
		yCondition = normalizedLine.y >= 0.0f ? start.y <= end.y : start.y >= end.y;
		//CPURenderer::Log("Drawing pixel point x: {}, y: {} at screen location {}", startX, startY, frameBufferIndex);
	}
}

static void DrawTriangle(Triangle2D& triangle, uint32_t* frameBuffer, const GameConfig& config) {
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

	Matrix3x3 viewportProjectionMatrix = Matrix3x3::GetScaled({ config.VIEWPORT_WIDTH * 0.5f , config.VIEWPORT_HEIGHT * 0.5f }) * Matrix3x3::GetTranslated({ 1.0f, 1.0f });
	Matrix3x3 ndsProjectionMatrix = Matrix3x3::GetTranslated({ -1.0f, -1.0f }) * Matrix3x3::GetScaled({ 2.0f / config.WIDTH, 2.0f / config.HEIGHT});
	Matrix3x3 transform = Matrix3x3::GetTranslated(triangle.pos) * Matrix3x3::GetRotated(triangle.rotate) * Matrix3x3::GetScaled(triangle.size);
	Matrix3x3 projection = viewportProjectionMatrix * ndsProjectionMatrix * transform;

	Vector2 a = triangle.a;
	Vector2 b = triangle.b;
	Vector2 c = triangle.c;

	a = projection * a;
	b = projection * b;
	c = projection * c;

	Vector2 caLine = (c - a).GetNormalized();

	Vector2 cbLine = (c - b).GetNormalized();

	auto hasNotReachedLineEnd = [](Vector2 line, Vector2 a, Vector2 b) -> bool {
		return (line.x >= 0.0f ? a.x <= b.x : a.x >= b.x) && (line.y >= 0.0f ? a.y <= b.y : a.y >= b.y);
	};

	bool keepDrawing = hasNotReachedLineEnd(caLine, a, c) && hasNotReachedLineEnd(cbLine, b, c);

	DrawLine(a, b, frameBuffer, config);
	DrawLine(a, c, frameBuffer, config);
	DrawLine(b, c, frameBuffer, config);

	while (keepDrawing) {
		a.x += caLine.x;
		a.y += caLine.y;

		b.x += cbLine.x;
		b.y += cbLine.y;
			
		DrawLine(a, b, frameBuffer, config);

		keepDrawing = hasNotReachedLineEnd(caLine, a, c) && hasNotReachedLineEnd(cbLine, b, c);
	}

	return;
}

static void DrawQuad(Quad2D& quad, uint32_t* frameBuffer, const GameConfig& config) {
	auto& indeces = quad.indeces;
	if (indeces.size() == 0 || indeces.size() % 3 != 0) {
		CPURenderer::Log("Not enough indeces provided");
		return;
	}

	for (int i = 0; i < indeces.size(); i += 3) {
		Triangle2D triangle = Triangle2D::Create(quad.points[indeces[i]], quad.points[indeces[i + 1]], quad.points[indeces[i + 2]], quad.pos, quad.size, quad.rotate);
		DrawTriangle(triangle, frameBuffer, config);
	}
}

static void DrawCircle(Circle2D circle, uint32_t* frameBuffer, const GameConfig& config) {
	constexpr int points = 360;
	float angle = 1.0f;
	float radius = 1.0f;

	Vector2 centerPoint = { 0.0f, 0.0f };
	Vector2 secondPoint = { 0.0f, centerPoint.y + radius };
	Vector2 prevPoint = secondPoint;

	for (int i = 0; i < 360; i++) {
		float s = std::sin(angle);
		float c = std::cos(angle);
		Vector2 thirdPoint = { secondPoint.x * c - s * secondPoint.y, secondPoint.x * s + secondPoint.y * c };
		Triangle2D triangle = Triangle2D::Create(centerPoint, prevPoint, thirdPoint, circle.center, circle.size, 0.0f);
		DrawTriangle(triangle, frameBuffer, config);
		prevPoint = thirdPoint;
		angle += 1.0f;
	}
}
	
int main() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		CPURenderer::Log("Failed to init sdl video");
		return -1;
	}

	CPURenderer::Log("SDL init successfully");

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	GameConfig config;

	if (!SDL_CreateWindowAndRenderer(
		"CPU Renderer",
		config.WIDTH,
		config.HEIGHT,
		SDL_WINDOW_RESIZABLE,
		&window,
		&renderer)) {
		CPURenderer::Log("{}", SDL_GetError());
		return -1;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, config.WIDTH, config.HEIGHT);

	uint32_t* frameBuffer = new uint32_t[config.WIDTH * config.HEIGHT]{};

	Vector2 bottomLeft = { -0.5f, -0.5f };
	Vector2 topLeft = { -0.5f, 0.5f };
	Vector2 bottomRight = { 0.5f, -0.5f };
	Vector2 topRight = { 0.5f, 0.5f };

	Quad2D quad = Quad2D::Create({ static_cast<float>(config.VIEWPORT_WIDTH), static_cast<float>(config.VIEWPORT_HEIGHT) }, { 800.0f, 800.0f }, 0.0f, { bottomLeft, topLeft, bottomRight, topRight }, { 0, 1, 2, 1, 2, 3});

	/*Triangle2D triangle = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { WIDTH * 0.5f, HEIGHT * 0.5f }, { 500.0f, 500.0f }, 10.0f);
	Triangle2D triangle2 = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { 200.0f, 200.0f }, { 10.0f, 10.0f }, 0.0f);

	DrawTriangle(triangle, frameBuffer, WIDTH, HEIGHT);
	DrawTriangle(triangle2, frameBuffer, WIDTH, HEIGHT);*/

	//DrawCircle(Circle2D::Create({ WIDTH * 0.5f, HEIGHT * 0.5f }, { 50.0f, 50.0f }), frameBuffer, WIDTH, HEIGHT);

	SDL_UpdateTexture(texture, nullptr, frameBuffer, config.WIDTH * sizeof(uint32_t));

	bool running = true;

	while (running) {
		std::fill(frameBuffer, frameBuffer + config.WIDTH * config.HEIGHT, 0u);

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
		
		DrawQuad(quad, frameBuffer, config);

		SDL_UpdateTexture(texture, nullptr, frameBuffer, config.WIDTH * sizeof(uint32_t));

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	SDL_Quit();

	delete[] frameBuffer;
	frameBuffer = nullptr;

	return 0;
}
