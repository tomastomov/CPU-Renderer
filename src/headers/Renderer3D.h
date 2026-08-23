#pragma once
#include <Vector3.h>
#include <Utils.h>
#include <GameConfig.h>
#include <Matrix4x4.h>
#include "CustomMath.h"

namespace CPURenderer {
	class Renderer3D {
	private:
		static void DrawTriangle(Vector3 a, Vector3 b, Vector3 c, uint32_t* frameBuffer, float* depthBuffer, const GameConfig& config, uint32_t color, Vector3 pos) {
			Vector3 size = { 500.0f, 500.0f, 50.0f };

			Matrix4x4 viewportProjectionMatrix = Matrix4x4::GetScaled({ config.VIEWPORT_WIDTH * 0.5f , config.VIEWPORT_HEIGHT * 0.5f, 1.0f}) * Matrix4x4::GetTranslated({ 1.0f, 1.0f, 0.0f });
			Matrix4x4 ndsProjectionMatrix = Matrix4x4::GetTranslated({ -1.0f, -1.0f, 0.0f }) * Matrix4x4::GetScaled({ 2.0f / config.WIDTH, 2.0f / config.HEIGHT, 1.0f });
			Matrix4x4 transform = Matrix4x4::GetTranslated(pos) * Matrix4x4::GetScaled(size);
			Matrix4x4 projection = viewportProjectionMatrix * ndsProjectionMatrix * transform;

			a = projection * a;
			b = projection * b;
			c = projection * c;

			int minX = std::max(0, static_cast<int>(std::floor(std::min({ a.x, b.x, c.x }))));
			int maxX = std::min(config.WIDTH - 1,
				static_cast<int>(std::ceil(std::max({ a.x, b.x, c.x }))));

			int minY = std::max(0, static_cast<int>(std::floor(std::min({ a.y, b.y, c.y }))));
			int maxY = std::min(config.HEIGHT - 1,
				static_cast<int>(std::ceil(std::max({ a.y, b.y, c.y }))));

			for (int x = minX; x <= maxX; x++)
			{
				for (int y = minY; y <= maxY; y++)
				{
					auto bary = CustomMath::GetBarycentric({ a.x, a.y }, { b.x, b.y }, { c.x, c.y }, { static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f });

					int index = y * config.WIDTH + x;

					if (bary.x >= 0.0f &&
						bary.y >= 0.0f &&
						bary.z >= 0.0f)
					{
						float z =
							bary.x * a.z +
							bary.y * b.z +
							bary.z * c.z;

						if (z > depthBuffer[index]) {
							depthBuffer[index] = z;
							frameBuffer[index] = color;
						}
					}
				}
			}
		}
	public:
		static void DrawTethradon(Vector3 a, Vector3 b, Vector3 c, Vector3 d, uint32_t* frameBuffer, float* depthBuffer, const GameConfig& config) {
			DrawTriangle(a, b, c, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), {550.0f, 500.0f, 500.0f});
			DrawTriangle({ a.x, a.y, a.z + 2.0f }, { b.x, b.y, b.z + 2.0f }, { c.x, c.y, c.z + 2.0f }, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 255, 0), { 500.0f, 500.0f, 500.0f });
		};
		static void DrawCube() {

		};
	};
}