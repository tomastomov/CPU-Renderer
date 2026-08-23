#pragma once
#include <Vector3.h>
#include <Utils.h>
#include <CustomMath.h>
#include <GameConfig.h>
#include <Matrix4x4.h>

namespace CPURenderer {
	class Renderer3D {
	private:
		static void DrawTriangle(Vector3 a, Vector3 b, Vector3 c, uint32_t* frameBuffer, const GameConfig& config, uint32_t color) {
			Vector3 pos = { 500.0f, 500.0f, 500.0f };
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

			for (int x = minX; x <= maxX; x++) {
				for (int y = minY; y <= maxY; y++) {
					if (CustomMath::IsPointInsideTriangleFast({ a.x, a.y }, { b.x, b.y }, { c.x, c.y }, { static_cast<float>(x), static_cast<float>(y) })) {
						frameBuffer[y * config.WIDTH + x] = color;
					}
				}
			}
		}
	public:
		static void DrawTethradon(Vector3 a, Vector3 b, Vector3 c, Vector3 d, uint32_t* frameBuffer, const GameConfig& config) {
			DrawTriangle(a, b, c, frameBuffer, config, Utils::GetColorFromARGB(255, 0, 255, 0));
		};
		static void DrawCube() {

		};
	};
}