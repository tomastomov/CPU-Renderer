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
		static void DrawTriangle(Vertex3 a, Vertex3 b, Vertex3 c, uint32_t* frameBuffer, float* depthBuffer, const GameConfig& config, Texture& texture, Vector3 pos, Vector3 size, Vector3 rotate, Camera& camera) {
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
				Matrix4x4::GetRotatedAroundX(-camera.rotate.x) *
				Matrix4x4::GetRotatedAroundY(-camera.rotate.y) *
				Matrix4x4::GetTranslated({
					-camera.pos.x,
					-camera.pos.y,
					-camera.pos.z
					}) *
				transform;

			Matrix4x4 projection =
				viewportProjectionMatrix *
				ndsProjectionMatrix;

			Vector3 vecA = modelView * a.vec;
			Vector3 vecB = modelView * b.vec;
			Vector3 vecC = modelView * c.vec;

			constexpr float nearPlane = 1.0f;

			if (vecA.z <= nearPlane ||
				vecB.z <= nearPlane ||
				vecC.z <= nearPlane)
			{
				return;
			}


			// Perspective
			float focalPoint = 500.0f;

			float halfWidth = config.WIDTH * 0.5f;
			float halfHeight = config.HEIGHT * 0.5f;

			vecA.x = halfWidth + vecA.x * focalPoint / vecA.z;
			vecA.y = halfHeight + vecA.y * focalPoint / vecA.z;

			vecB.x = halfWidth + vecB.x * focalPoint / vecB.z;
			vecB.y = halfHeight + vecB.y * focalPoint / vecB.z;

			vecC.x = halfWidth + vecC.x * focalPoint / vecC.z;
			vecC.y = halfHeight + vecC.y * focalPoint / vecC.z;

			vecA = projection * vecA;
			vecB = projection * vecB;
			vecC = projection * vecC;

			int minX = std::max(0, static_cast<int>(std::floor(std::min({ vecA.x, vecB.x, vecC.x }))));
			int maxX = std::min(config.WIDTH - 1,
				static_cast<int>(std::ceil(std::max({ vecA.x, vecB.x, vecC.x }))));

			int minY = std::max(0, static_cast<int>(std::floor(std::min({ vecA.y, vecB.y, vecC.y }))));
			int maxY = std::min(config.HEIGHT - 1,
				static_cast<int>(std::ceil(std::max({ vecA.y, vecB.y, vecC.y }))));

			float denom =
				(vecB.y - vecC.y) * (vecA.x - vecC.x) +
				(vecC.x - vecB.x) * (vecA.y - vecC.y);

			if (std::abs(denom) < 0.000001f)
			{
				CPURenderer::Log("Degen triangle");
				return;
			}

			for (int x = minX; x <= maxX; x++)
			{
				for (int y = minY; y <= maxY; y++)
				{
					auto bary = CustomMath::GetBarycentric({ vecA.x, vecA.y }, { vecB.x, vecB.y }, { vecC.x, vecC.y }, { static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f });

					int index = y * config.WIDTH + x;

					if (bary.x >= 0.0f &&
						bary.y >= 0.0f &&
						bary.z >= 0.0f)
					{
						float z =
							bary.x * vecA.z +
							bary.y * vecB.z +
							bary.z * vecC.z;

						if (z < depthBuffer[index])
						{
							float u =
								bary.x * a.u +
								bary.y * b.u +
								bary.z * c.u;

							float v =
								bary.x * a.v +
								bary.y * b.v +
								bary.z * c.v;

							auto pixel = texture.GetPixelUV(u, v);

							if (pixel.a <= 0) {
								continue;
							}

							depthBuffer[index] = z;

							frameBuffer[index] =
								Utils::GetColorFromARGB(
									pixel.a,
									pixel.r,
									pixel.g,
									pixel.b
								);
						}
					}
				}
			}
		}

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
				Matrix4x4::GetRotatedAroundX(-camera.rotate.x) *
				Matrix4x4::GetRotatedAroundY(-camera.rotate.y) *
				Matrix4x4::GetTranslated({
					-camera.pos.x,
					-camera.pos.y,
					-camera.pos.z
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

			a.x = halfWidth + a.x * focalPoint / a.z;
			a.y = halfHeight + a.y * focalPoint / a.z;

			b.x = halfWidth + b.x * focalPoint / b.z;
			b.y = halfHeight + b.y * focalPoint / b.z;

			c.x = halfWidth + c.x * focalPoint / c.z;
			c.y = halfHeight + c.y * focalPoint / c.z;

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

		static void DrawCube(
			CubeMesh& cube,
			uint32_t* frameBuffer,
			float* depthBuffer,
			const GameConfig& config,
			Camera& camera,
			Texture& texture)
		{
			// Front
			DrawTriangle(
				cube.frontA, cube.frontB, cube.frontC,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			DrawTriangle(
				cube.frontA, cube.frontC, cube.frontD,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			// Right
			DrawTriangle(
				cube.rightA, cube.rightB, cube.rightC,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			DrawTriangle(
				cube.rightA, cube.rightC, cube.rightD,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			// Back
			DrawTriangle(
				cube.backA, cube.backB, cube.backC,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			DrawTriangle(
				cube.backA, cube.backC, cube.backD,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			// Left
			DrawTriangle(
				cube.leftA, cube.leftB, cube.leftC,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			DrawTriangle(
				cube.leftA, cube.leftC, cube.leftD,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			// Top
			DrawTriangle(
				cube.topA, cube.topB, cube.topC,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			DrawTriangle(
				cube.topA, cube.topC, cube.topD,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			// Bottom
			DrawTriangle(
				cube.bottomA, cube.bottomB, cube.bottomC,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);

			DrawTriangle(
				cube.bottomA, cube.bottomC, cube.bottomD,
				frameBuffer, depthBuffer, config,
				texture, cube.pos, cube.size, cube.rotate, camera
			);
		}
	};
}