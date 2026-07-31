#include "pch.h"
#include <Log.h>
#include <SDL3/SDL.h>
#include <Vector2.h>

using CPURenderer::Vector2;

//TODO::fix projection matrix and rotation

struct Matrix3x3 {
	float arr[3][3];
	Matrix3x3(const float(&a)[3][3]) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				arr[i][j] = a[i][j];
			}
		}
	}

	void Scale(const float scaleFactor) {
		for (int i = 0; i < 2; i++) {
			arr[i][i] *= scaleFactor;
		}
	}

	void Scale(const Vector2 v) {
		arr[0][0] *= v.x;
		arr[1][1] *= v.y;
	}

	void Rotate(const float angle) {
		static constexpr float epsilon = 1e-6f;
		float radians = angle * std::numbers::pi_v<float> / 180.0f;
		float c = std::cos(radians);
		float s = std::sin(radians);

		if (std::abs(s) < epsilon) {
			s = 0.0f;
		}

		if (std::abs(c) < epsilon) {
			c = 0.0f;
		}

		arr[0][0] = c;
		arr[0][1] = -s;
		arr[1][0] = s;
		arr[1][1] = c;
	}

	void Translate(const Vector2 v) {
		arr[0][2] = v.x;
		arr[1][2] = v.y;
	}

	static Matrix3x3 GetIdentity() {
		static constexpr float identity[3][3] = {
			{1.0f, 0.0f, 0.0f,}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
		};

		return Matrix3x3(identity);
	}

	static Matrix3x3 GetTranslated(const Vector2 v) {
		Matrix3x3 identity = GetIdentity();
		identity.Translate(v);

		return identity;
	}

	static Matrix3x3 GetScaled(const Vector2 v) {
		Matrix3x3 identity = GetIdentity();
		identity.Scale(v);

		return identity;
	}

	static Matrix3x3 GetRotated(float angle) {
		Matrix3x3 identity = GetIdentity();
		identity.Rotate(angle);

		return identity;
	}

	void Print() {
		CPURenderer::Log("First row: {} {} {}", arr[0][0], arr[0][1], arr[0][2]);
		CPURenderer::Log("Second row: {} {} {}", arr[1][0], arr[1][1], arr[1][2]);
		CPURenderer::Log("Third row: {} {} {}", arr[2][0], arr[2][1], arr[2][2]);
	}

	Matrix3x3 operator *(const Matrix3x3& other) {
		float result[3][3];

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				result[i][j] = (arr[i][0] * other.arr[0][j]) + (arr[i][1] * other.arr[1][j]) + (arr[i][2] * other.arr[2][j]);
			}
		}

		return Matrix3x3(result);
	}

	Vector2 operator *(const Vector2 v) {
		Vector2 res;

		res.x = (arr[0][0] * v.x) + (arr[0][1] * v.y) + (arr[0][2] * 1);
		res.y = (arr[1][0] * v.x) + (arr[1][1] * v.y) + (arr[1][2] * 1);

		return res;
	}
};

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

static void DrawLine(Vector2 a, Vector2 b, uint32_t* frameBuffer, const int WIDTH, const int HEIGHT) {
	CPURenderer::Log("Processing point x: {}, y: {} and x1: {}, y1: {} ", a.x, a.y, b.x, b.y);
	Vector2 line = b - a;
	Vector2 normalizedLine = line.GetNormalized();

	float startX = a.x;
	float startY = a.y;
	float endX = b.x;
	float endY = b.y;

	bool xCondition = normalizedLine.x >= 0.0f ? startX <= endX : startX >= endX;
	bool yCondition = normalizedLine.y >= 0.0f ? startY <= endY : startY >= endY;

	while (xCondition && yCondition) {
		startX += normalizedLine.x;
		startY += normalizedLine.y;
		int roundedX = std::round(startX);
		int roundedY = std::round(startY);

		if (roundedX >= WIDTH || roundedY >= HEIGHT || roundedX < 0 || roundedY < 0) {
			CPURenderer::Log("Skipping point - x: {}, y: {}", roundedX, roundedY);
			return;
		}

		int frameBufferIndex = roundedY * WIDTH + roundedX;

		frameBuffer[frameBufferIndex] = GetColorFromARGB(255, 0, 0, 255);

		xCondition = normalizedLine.x >= 0.0f ? startX <= endX : startX >= endX;
		yCondition = normalizedLine.y >= 0.0f ? startY <= endY : startY >= endY;
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

	//CPURenderer::Log("Points initially a: x: {} y: {}, b: x: {}, y: {}, c: x: {}, y: {}", a.x, a.y, b.x, b.y, c.x, c.y);

	a = transform * a;
	b = transform * b;
	c = transform * c;

	CPURenderer::Log("Points after rotation a: ({}, {}), b: ({}, {}), c: ({}, {})", a.x, a.y, b.x, b.y, c.x, c.y);

	/*if (b.y > c.y) {
		Vector2 temp = c;
		c = b;
		b = temp;
	}

	if (a.y > c.y) {
		Vector2 temp = c;
		c = a;
		a = temp;
	}*/

	Vector2 caLine = (c - a).GetNormalized();

	Vector2 cbLine = (c - b).GetNormalized();

	auto hasNotReachedLineEnd = [](Vector2 line, Vector2 a, Vector2 b) -> bool {
		return (line.x >= 0.0f ? a.x <= b.x : a.x >= b.x) && (line.y >= 0.0f ? a.y <= b.y : a.y >= b.y);
	};

	bool keepDrawing = hasNotReachedLineEnd(caLine, a, c) && hasNotReachedLineEnd(cbLine, b, c);

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

	Triangle2D triangle = Triangle2D::Create(bottomLeft, bottomRight, topLeft, { 500, 500 }, { 500.0f, 500.0f}, 285.0f);

	DrawTriangle(triangle, frameBuffer, WIDTH, HEIGHT);

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
