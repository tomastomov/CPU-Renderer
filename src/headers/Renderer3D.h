#pragma once
#include <Vector3.h>
#include <Utils.h>
#include <GameConfig.h>
#include <Matrix4x4.h>
#include "CustomMath.h"
#include "Tethradon.h"
#include <Cube.h>
#include "Camera.h"

namespace CPURenderer {
	class Renderer3D {
	private:
		static void DrawTriangle(Vector3 a, Vector3 b, Vector3 c, uint32_t* frameBuffer, float* depthBuffer, const GameConfig& config, uint32_t color, Vector3 pos, Vector3 size, Vector3 rotate, Camera& camera) {
			Matrix4x4 rotation =
				Matrix4x4::GetRotatedAroundZ(rotate.z) *
				Matrix4x4::GetRotatedAroundY(rotate.y) *
				Matrix4x4::GetRotatedAroundX(rotate.x);

			Matrix4x4 viewportProjectionMatrix =
				Matrix4x4::GetScaled({
					config.VIEWPORT_WIDTH * 0.5f,
					config.VIEWPORT_HEIGHT * 0.5f,
					1.0f
					}) *
				Matrix4x4::GetTranslated({
					1.0f,
					1.0f,
					0.0f
					});

			Matrix4x4 ndsProjectionMatrix =
				Matrix4x4::GetTranslated({
					-1.0f,
					-1.0f,
					0.0f
					}) *
				Matrix4x4::GetScaled({
					2.0f / config.WIDTH,
					2.0f / config.HEIGHT,
					1.0f
					});

			Matrix4x4 transform =
				Matrix4x4::GetTranslated(pos) *
				rotation *
				Matrix4x4::GetScaled(size);

			Matrix4x4 modelView =
				Matrix4x4::GetTranslated({
					camera.x,
					camera.y,
					camera.z
					}) *
				transform;

			Matrix4x4 projection =
				viewportProjectionMatrix *
				ndsProjectionMatrix;

			a = modelView * a;
			b = modelView * b;
			c = modelView * c;


			constexpr float nearPlane = 1.0f;

			if (a.z <= nearPlane ||
				b.z <= nearPlane ||
				c.z <= nearPlane)
			{
				return;
			}


			// Perspective
			float focalPoint = 500.0f;

			float halfWidth = config.WIDTH * 0.5f;
			float halfHeight = config.HEIGHT * 0.5f;

			a.x = halfWidth + (a.x - halfWidth) * focalPoint / a.z;
			a.y = halfHeight + (a.y - halfHeight) * focalPoint / a.z;

			b.x = halfWidth + (b.x - halfWidth) * focalPoint / b.z;
			b.y = halfHeight + (b.y - halfHeight) * focalPoint / b.z;

			c.x = halfWidth + (c.x - halfWidth) * focalPoint / c.z;
			c.y = halfHeight + (c.y - halfHeight) * focalPoint / c.z;


			a = projection * a;
			b = projection * b;
			c = projection * c;

			int minX = std::max(0, static_cast<int>(std::floor(std::min({ a.x, b.x, c.x }))));
			int maxX = std::min(config.WIDTH - 1,
				static_cast<int>(std::ceil(std::max({ a.x, b.x, c.x }))));

			int minY = std::max(0, static_cast<int>(std::floor(std::min({ a.y, b.y, c.y }))));
			int maxY = std::min(config.HEIGHT - 1,
				static_cast<int>(std::ceil(std::max({ a.y, b.y, c.y }))));

			float denom =
				(b.y - c.y) * (a.x - c.x) +
				(c.x - b.x) * (a.y - c.y);

			if (std::abs(denom) < 0.000001f)
			{
				CPURenderer::Log("Degen triangle");
				return;
			}

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

						if (z < depthBuffer[index]) {
							depthBuffer[index] = z;
							frameBuffer[index] = color;
						}
					}
				}
			}
		}
	public:
		static void DrawTethradon(Tethradon& tethradon, uint32_t* frameBuffer, float* depthBuffer, const GameConfig& config, Camera& camera) {
			DrawTriangle(tethradon.a, tethradon.b, tethradon.c, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), tethradon.pos, tethradon.size, tethradon.rotate, camera);
			DrawTriangle(tethradon.a, tethradon.c, tethradon.d, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 255, 0), tethradon.pos, tethradon.size, tethradon.rotate, camera);
			DrawTriangle(tethradon.b, tethradon.c, tethradon.d, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 0, 255), tethradon.pos, tethradon.size, tethradon.rotate, camera);
			DrawTriangle(tethradon.a, tethradon.b, tethradon.d, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 120, 120), tethradon.pos, tethradon.size, tethradon.rotate, camera);
		};
		static void DrawCube(Cube& cube, uint32_t* frameBuffer, float* depthBuffer, const GameConfig& config, Camera& camera) {
			DrawTriangle(cube.a, cube.b, cube.c, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.a, cube.c, cube.d, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.b, cube.b1, cube.c1, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 255, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.b, cube.c, cube.c1, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 255, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.a1, cube.b1, cube.c1, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 0, 255), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.a1, cube.d1, cube.c1, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 0, 255), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.a, cube.a1, cube.d, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 120, 120), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.d, cube.a1, cube.d1, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 0, 120, 120), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.a, cube.a1, cube.b, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.b, cube.b1, cube.a1, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.d, cube.d1, cube.c, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), cube.pos, cube.size, cube.rotate, camera);
			DrawTriangle(cube.d1, cube.c1, cube.c, frameBuffer, depthBuffer, config, Utils::GetColorFromARGB(255, 255, 0, 0), cube.pos, cube.size, cube.rotate, camera);
		};
	};
}