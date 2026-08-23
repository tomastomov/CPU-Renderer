#pragma once
#include <Triangle2D.h>
#include <GameConfig.h>
#include <Texture.h>
#include <Quad2D.h>
#include "Vertex2.h"
#include "Vector2.h"
#include <CustomMath.h>

namespace CPURenderer {
	class Renderer2D {
	public:
		static inline void DrawTriangle(Triangle2D& triangle, const GameConfig& config, uint32_t* frameBuffer, Texture& texture) {
			double abLen = (triangle.b.vector - triangle.a.vector).GetLength();
			double bcLen = (triangle.c.vector - triangle.b.vector).GetLength();
			double caLen = (triangle.a.vector - triangle.c.vector).GetLength();

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

			Vector2 a = triangle.a.vector;
			Vector2 b = triangle.b.vector;
			Vector2 c = triangle.c.vector;

			a = projection * a;
			b = projection * b;
			c = projection * c;

			Vector2 caLine = (c - a).GetNormalized();

			Vector2 cbLine = (c - b).GetNormalized();

			auto hasNotReachedLineEnd = [](Vector2 line, Vector2 a, Vector2 b) -> bool {
				return (line.x >= 0.0f ? a.x <= b.x : a.x >= b.x) && (line.y >= 0.0f ? a.y <= b.y : a.y >= b.y);
				};

			bool keepDrawing = hasNotReachedLineEnd(caLine, a, c) && hasNotReachedLineEnd(cbLine, b, c);

			Vertex2 v_a = { a, triangle.a.u, triangle.a.v };
			Vertex2 v_b = { b, triangle.b.u, triangle.b.v };
			Vertex2 v_c = { c, triangle.c.u, triangle.c.v };

			DrawLine(a, b, frameBuffer, config, v_a, v_b, v_c, texture);
			DrawLine(a, c, frameBuffer, config, v_a, v_b, v_c, texture);
			DrawLine(b, c, frameBuffer, config, v_a, v_b, v_c, texture);

			Vector2 tempA = a;
			Vector2 tempB = b;
			Vector2 tempC = c;

			std::vector<std::future<void>> jobs;
			jobs.reserve(10000);

			while (keepDrawing) {
				tempA.x += caLine.x;
				tempA.y += caLine.y;

				tempB.x += cbLine.x;
				tempB.y += cbLine.y;

				jobs.push_back(ThreadPool::GetInstance().SubmitJob(
					[tempA, tempB, &frameBuffer, &config, v_a, v_b, v_c, &texture] {
						DrawLine(
							tempA,
							tempB,
							frameBuffer,
							config,
							v_a, v_b, v_c,
							texture
						);
					}
				));

				keepDrawing = hasNotReachedLineEnd(caLine, tempA, c) && hasNotReachedLineEnd(cbLine, tempB, c);
			}

			for (auto& job : jobs) {
				job.wait();
			}

			return;
		}
		static __forceinline void DrawLine(Vector2 a, Vector2 b, uint32_t* frameBuffer, const GameConfig& config, Vertex2 v_a, Vertex2 v_b, Vertex2 v_c, Texture& texture) {
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

				DrawPixel(std::ceil(start.x), std::ceil(start.y), frameBuffer, config, end, normalizedLine, v_a, v_b, v_c, texture);
				DrawPixel(std::floor(start.x), std::floor(start.y), frameBuffer, config, end, normalizedLine, v_a, v_b, v_c, texture);
				DrawPixel(std::ceil(start.x), std::floor(start.y), frameBuffer, config, end, normalizedLine, v_a, v_b, v_c, texture);
				DrawPixel(std::floor(start.x), std::ceil(start.y), frameBuffer, config, end, normalizedLine, v_a, v_b, v_c, texture);

				xCondition = normalizedLine.x >= 0.0f ? start.x <= end.x : start.x >= end.x;
				yCondition = normalizedLine.y >= 0.0f ? start.y <= end.y : start.y >= end.y;
				//CPURenderer::Log("Drawing pixel point x: {}, y: {} at screen location {}", startX, startY, frameBufferIndex);
			}
		}
		static inline void DrawQuad(Quad2D& quad, uint32_t* frameBuffer, const GameConfig& config, Texture& texture) {
			auto& indeces = quad.indeces;
			if (indeces.size() == 0 || indeces.size() % 3 != 0) {
				CPURenderer::Log("Not enough indeces provided");
				return;
			}

			std::vector<std::future<void>> jobs;
			jobs.reserve(2);

			for (int i = 0; i < indeces.size(); i += 3) {
				Triangle2D triangle = Triangle2D::Create(quad.points[indeces[i]], quad.points[indeces[i + 1]], quad.points[indeces[i + 2]], quad.pos, quad.size, quad.rotate);

				jobs.push_back(ThreadPool::GetInstance().SubmitJob(
					[triangle, &frameBuffer, &config, &texture]() mutable {
						DrawTriangle(triangle, config, frameBuffer, texture);
					}
				));
			}

			for (const auto& job : jobs) {
				job.wait();
			}
		}
		static __forceinline void DrawPixel(int x, int y, uint32_t* frameBuffer, const GameConfig& config, Vector2 end, Vector2 line, Vertex2 a, Vertex2 b, Vertex2 c, Texture& texture) {
			int endX = std::floor(end.x);
			int endY = std::floor(end.y);

			bool isOnLine = (line.x <= 0 ? x >= endX : x <= endX) && (line.y <= 0 ? y >= endY : y <= endY);

			if (!isOnLine) {
				//CPURenderer::Log("({}, {}) is not on line ({}, {}) with slope ({}, {})", x, y, endX, endY, line.x, line.y);
				return;
			}

			Vector2 p = { static_cast<float>(x), static_cast<float>(y) };

			if (!CustomMath::IsPointInsideTriangleFast(a.vector, b.vector, c.vector, p)) {
				return;
			}

			bool isInsideWorld = x < config.WIDTH && y < config.HEIGHT && x >= 0 && y >= 0 && isOnLine;
			bool isInsideViewPort = x < config.VIEWPORT_WIDTH && y < config.VIEWPORT_HEIGHT && x >= 0 && y >= 0;

			if (isInsideWorld && isInsideViewPort) {
				int frameBufferIndex = y * config.WIDTH + x;

				auto uvWeights = CustomMath::GetUVWeightsFast(a, b, c, p);

				Pixel pixel = texture.GetPixel(uvWeights.first * (texture.GetWidth() - 1), uvWeights.second * (texture.GetHeight() - 1));

				frameBuffer[frameBufferIndex] = Utils::GetColorFromARGB(pixel.a, pixel.r, pixel.g, pixel.b);
			}
		}
	};
}

//static void DrawCircle(Circle2D circle, uint32_t* frameBuffer, const GameConfig& config, Texture& texture) {
//	static constexpr float degreesToRadians =
//		std::numbers::pi_v<float> / 180.0f;
//
//	constexpr int points = 360;
//	float angle = 1.0f;
//	float radius = 1.0f;
//
//	Vector2 centerPoint = { 0.0f, 0.0f };
//	Vector2 secondPoint = { 0.0f, centerPoint.y + radius };
//	Vector2 prevPoint = secondPoint;
//
//	for (int i = 0; i < 360; i++) {
//		float s = std::sin(angle * degreesToRadians);
//		float c = std::cos(angle * degreesToRadians);
//		Vector2 thirdPoint = { secondPoint.x * c - s * secondPoint.y, secondPoint.x * s + secondPoint.y * c };
//		Triangle2D triangle = Triangle2D::Create(centerPoint, prevPoint, thirdPoint, circle.center, circle.size, 0.0f);
//		DrawTriangle(triangle, frameBuffer, config, texture);
//		prevPoint = thirdPoint;
//		angle += 1.0f;
//	}
//}