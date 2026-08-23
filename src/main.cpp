#include "pch.h"
#include <Log.h>
#include <SDL3/SDL.h>
#include <Vector2.h>
#include <Matrix3x3.h>
#include <Quad2D.h>
#include <Triangle2D.h>
#include <GameConfig.h>
#include <Texture.h>
#include <Vertex2.h>
#include <ThreadPool.h>
#include <Utils.h>
#include <Renderer2D.h>
#include <Renderer3D.h>

using CPURenderer::Vector2;
using CPURenderer::Matrix3x3;
using CPURenderer::GameConfig;
using CPURenderer::Texture;
using CPURenderer::Vertex2;
using CPURenderer::Triangle2D;
using CPURenderer::Quad2D;
using CPURenderer::ThreadPool;
using CPURenderer::Utils;
using CPURenderer::Renderer2D;
using CPURenderer::Renderer3D;

//TODO:: figure out on how to know if a point is inside a triangle or not
//TODO:: check if it is inside viewport

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

	ThreadPool threadPool(8);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, config.WIDTH, config.HEIGHT);

	uint32_t* frameBuffer = new uint32_t[config.WIDTH * config.HEIGHT]{};
	float* depthBuffer = new float[config.WIDTH * config.HEIGHT]{};

	Vector2 bottomLeft = { -0.5f, -0.5f };
	Vector2 topLeft = { -0.5f, 0.5f };
	Vector2 bottomRight = { 0.5f, -0.5f };
	Vector2 topRight = { 0.5f, 0.5f };

	Quad2D quad = Quad2D::Create({ static_cast<float>(config.VIEWPORT_WIDTH) * 0.5f, static_cast<float>(config.VIEWPORT_HEIGHT) * 0.5f }, { 800.0f, 800.0f }, 45.0f, { { bottomLeft, 0.0f, 0.0f }, { topLeft, 0.0f, 1.0f }, { bottomRight, 1.0f, 0.0f }, { topRight, 1.0f, 1.0f} }, { 0, 1, 2, 1, 2, 3 });
	//Quad2D quad = Quad2D::Create({910, 540}, { 400.0f, 400.0f }, 0.0f, { bottomLeft, topLeft, bottomRight, topRight }, { 0, 1, 2, 1, 2, 3 });

	Triangle2D triangle = Triangle2D::Create({ bottomLeft, 0.0f, 0.0f }, { bottomRight, 1.0f, 0.0f }, { topLeft, 0.0f, 1.0f }, {config.WIDTH * 0.5f, config.HEIGHT * 0.5f}, {500.0f, 500.0f}, 10.0f);
	Triangle2D triangle2 = Triangle2D::Create({ bottomLeft, 0.0f, 0.0f }, { bottomRight, 1.0f, 0.0f }, { topLeft, 0.0f, 1.0f }, { 200.0f, 200.0f }, { 10.0f, 10.0f }, 0.0f);

	SDL_UpdateTexture(texture, nullptr, frameBuffer, config.WIDTH * sizeof(uint32_t));

	CPURenderer::Texture gorillaTexture("resources/gorilla.png");

	bool running = true;

	while (running) {
		std::fill(frameBuffer, frameBuffer + config.WIDTH * config.HEIGHT, 0u);
		std::fill(depthBuffer, depthBuffer + config.WIDTH * config.HEIGHT, -1e5);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_KEY_DOWN) {
				CPURenderer::Log("Received keydown event");
				if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
					running = false;
				}
				if (event.key.scancode == SDL_SCANCODE_D) {
					triangle.pos.x += 100.0f;
					quad.pos.x += 100.0f;
				}
				if (event.key.scancode == SDL_SCANCODE_A) {
					triangle.pos.x -= 100.0f;
					quad.pos.x -= 100.0f;
				}
				if (event.key.scancode == SDL_SCANCODE_W) {
					quad.pos.y -= 100.0f;
				}
				if (event.key.scancode == SDL_SCANCODE_S) {
					quad.pos.y += 100.0f;
				}
			}
			else if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		//Renderer2D::DrawQuad(quad, frameBuffer, config, gorillaTexture);

		Renderer3D::DrawTethradon({ -0.5f, -0.5f, 1.0f }, { 0.5f, -0.5f, 1.0f }, { -0.5f, 0.5f, 1.0f }, {}, frameBuffer, depthBuffer, config);

		//DrawCircle(Circle2D::Create({ config.WIDTH * 0.5f, config.HEIGHT * 0.5f }, { 100.0f, 100.0f }), frameBuffer, config);

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
