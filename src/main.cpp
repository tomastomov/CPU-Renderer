#include "pch.h"
#include <Log.h>
#include <SDL3/SDL.h>
#include <Vector2.h>
#include <Matrix3x3.h>
#include <Quad2D.h>
#include <Triangle2D.h>
#include <GameConfig.h>
#include <assert.h>

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

static double FindAngleByCosineLaw(
	double adjacentA,
	double adjacentB,
	double opposite)
{
	assert(adjacentA > 0.0);
	assert(adjacentB > 0.0);

	double cosine =
		(adjacentA * adjacentA +
			adjacentB * adjacentB -
			opposite * opposite)
		/ (2.0 * adjacentA * adjacentB);

	cosine = std::clamp(cosine, -1.0, 1.0);

	return std::acos(cosine);
}

struct TriangleAngleData {
	Vector2 a;
	Vector2 b;
	Vector2 c;

	double ab;
	double ac;
	double bc;

	double angleA;
	double angleB;
	double angleC;

	static TriangleAngleData CreateTriangleAngleData(
		Vector2 a,
		Vector2 b,
		Vector2 c)
	{
		TriangleAngleData data{};

		data.a = a;
		data.b = b;
		data.c = c;

		data.ab = (a - b).GetLength();
		data.ac = (a - c).GetLength();
		data.bc = (b - c).GetLength();

		data.angleA = FindAngleByCosineLaw(data.ab, data.ac, data.bc);
		data.angleB = FindAngleByCosineLaw(data.ab, data.bc, data.ac);
		data.angleC = FindAngleByCosineLaw(data.ac, data.bc, data.ab);

		return data;
	}
};

static bool IsPointInsideTriangle(TriangleAngleData& triangleAngleData, Vector2 point)
{
	const double ab = (triangleAngleData.a - triangleAngleData.b).GetLength();
	const double ac = (triangleAngleData.a - triangleAngleData.c).GetLength();
	const double bc = (triangleAngleData.b - triangleAngleData.c).GetLength();

	constexpr double distanceEpsilon = 1e-4;

	if (ab <= distanceEpsilon ||
		ac <= distanceEpsilon ||
		bc <= distanceEpsilon)
	{
		return false;
	}

	const double ha = (triangleAngleData.a - point).GetLength();
	const double hb = (triangleAngleData.b - point).GetLength();
	const double hc = (triangleAngleData.c - point).GetLength();

	if (ha <= distanceEpsilon ||
		hb <= distanceEpsilon ||
		hc <= distanceEpsilon)
	{
		return true;
	}

	const double bacAngle = triangleAngleData.angleA;
	const double abcAngle = triangleAngleData.angleB;
	const double bcaAngle = triangleAngleData.angleC;

	constexpr double angleEpsilon = 1e-2;

	const double hcbAngle = FindAngleByCosineLaw(bc, hc, hb);
	const double hcaAngle = FindAngleByCosineLaw(hc, ac, ha);

	double differenceA = (hcbAngle + hcaAngle) - bcaAngle;

	if ((hcbAngle + hcaAngle) - bcaAngle > angleEpsilon) {
		return false;
	}

	const double abhAngle = FindAngleByCosineLaw(ab, hb, ha);
	const double cbhAngle = FindAngleByCosineLaw(bc, hb, hc);

	if (abhAngle + cbhAngle > abcAngle + angleEpsilon)
		return false;

	const double habAngle = FindAngleByCosineLaw(ha, ab, hb);
	const double hacAngle = FindAngleByCosineLaw(ac, ha, hc);

	if (habAngle + hacAngle > bacAngle + angleEpsilon)
		return false;

	return true;
}

static void DrawPixel(int x, int y, uint32_t* frameBuffer, const GameConfig& config, Vector2 end, Vector2 line, TriangleAngleData& triangleAngleData) {
	int endX = std::floor(end.x);
	int endY = std::floor(end.y);

	bool isOnLine = (line.x <= 0 ? x >= endX : x <= endX) && (line.y <= 0 ? y >= endY : y <= endY);

	if (!isOnLine) {
		//CPURenderer::Log("({}, {}) is not on line ({}, {}) with slope ({}, {})", x, y, endX, endY, line.x, line.y);
		return;
	}

	Vector2 p = { static_cast<float>(x), static_cast<float>(y) };

	if (!IsPointInsideTriangle(triangleAngleData, p)) {
		return;
	}

	bool isInsideWorld = x < config.WIDTH && y < config.HEIGHT && x >= 0 && y >= 0 && isOnLine;
	bool isInsideViewPort = x < config.VIEWPORT_WIDTH && y < config.VIEWPORT_HEIGHT && x >= 0 && y >= 0;

	if (isInsideWorld && isInsideViewPort) {
		int frameBufferIndex = y * config.WIDTH + x;

		frameBuffer[frameBufferIndex] = GetColorFromARGB(255, 0, 0, 255);
	}
}

static void DrawLine(Vector2 a, Vector2 b, uint32_t* frameBuffer, const GameConfig& config, TriangleAngleData& triangleAngleData) {
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

		DrawPixel(std::ceil(start.x), std::ceil(start.y), frameBuffer, config, end, normalizedLine, triangleAngleData);
		DrawPixel(std::floor(start.x), std::floor(start.y), frameBuffer, config, end, normalizedLine, triangleAngleData);
		DrawPixel(std::ceil(start.x), std::floor(start.y), frameBuffer, config, end, normalizedLine, triangleAngleData);
		DrawPixel(std::floor(start.x), std::ceil(start.y), frameBuffer, config, end, normalizedLine, triangleAngleData);

		xCondition = normalizedLine.x >= 0.0f ? start.x <= end.x : start.x >= end.x;
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
	Matrix3x3 ndsProjectionMatrix = Matrix3x3::GetTranslated({ -1.0f, -1.0f }) * Matrix3x3::GetScaled({ 2.0f / config.WIDTH, 2.0f / config.HEIGHT });
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

	TriangleAngleData triangleAngleData = TriangleAngleData::CreateTriangleAngleData(a, b, c);

	DrawLine(a, b, frameBuffer, config, triangleAngleData);
	DrawLine(a, c, frameBuffer, config, triangleAngleData);
	DrawLine(b, c, frameBuffer, config, triangleAngleData);

	Vector2 tempA = a;
	Vector2 tempB = b;
	Vector2 tempC = c;

	while (keepDrawing) {
		tempA.x += caLine.x;
		tempA.y += caLine.y;

		tempB.x += cbLine.x;
		tempB.y += cbLine.y;

		DrawLine(tempA, tempB, frameBuffer, config, triangleAngleData);

		keepDrawing = hasNotReachedLineEnd(caLine, tempA, c) && hasNotReachedLineEnd(cbLine, tempB, c);
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
	static constexpr float degreesToRadians =
		std::numbers::pi_v<float> / 180.0f;

	constexpr int points = 360;
	float angle = 1.0f;
	float radius = 1.0f;

	Vector2 centerPoint = { 0.0f, 0.0f };
	Vector2 secondPoint = { 0.0f, centerPoint.y + radius };
	Vector2 prevPoint = secondPoint;

	for (int i = 0; i < 360; i++) {
		float s = std::sin(angle * degreesToRadians);
		float c = std::cos(angle * degreesToRadians);
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

	//Quad2D quad = Quad2D::Create({ static_cast<float>(config.VIEWPORT_WIDTH), static_cast<float>(config.VIEWPORT_HEIGHT) }, { 800.0f, 800.0f }, 0.0f, { bottomLeft, topLeft, bottomRight, topRight }, { 0, 1, 2, 1, 2, 3});
	//Quad2D quad = Quad2D::Create({910, 540}, { 400.0f, 400.0f }, 0.0f, { bottomLeft, topLeft, bottomRight, topRight }, { 0, 1, 2, 1, 2, 3 });

	Triangle2D triangle = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { config.WIDTH * 0.5f, config.HEIGHT * 0.5f }, { 500.0f, 500.0f }, 10.0f);
	Triangle2D triangle2 = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { 200.0f, 200.0f }, { 10.0f, 10.0f }, 0.0f);

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

		/*DrawTriangle(triangle, frameBuffer, config);
		DrawTriangle(triangle2, frameBuffer, config);*/

		DrawCircle(Circle2D::Create({ config.WIDTH * 0.5f, config.HEIGHT * 0.5f }, { 100.0f, 100.0f }), frameBuffer, config);

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
