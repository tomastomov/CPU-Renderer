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
#include <Tethradon.h>
#include "Cube.h"
#include <Camera.h>

using CPURenderer::Vector2;
using CPURenderer::Vector3;
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
using CPURenderer::Tethradon;
using CPURenderer::Cube;
using CPURenderer::Camera;
using CPURenderer::CubeMesh;

//TODO:: figure out on how to know if a point is inside a triangle or not
//TODO:: check if it is inside viewport

double GetTime()
{
	static const Uint64 frequency = SDL_GetPerformanceFrequency();
	return static_cast<double>(SDL_GetPerformanceCounter()) /
		static_cast<double>(frequency);
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
	CPURenderer::Texture zombieTexture("resources/zombie.png");

	Tethradon tethradon = { { -0.5f, -0.5f, 1.0f }, { 0.5f, -0.5f, 1.0f }, { -0.5f, 0.5f, 1.0f }, { -0.1667f, -0.1667f, 0.0f }, { 500.0f, 500.0f, 20.0f}, {500.0f, 500.0f, 500.0f}, { 0.0f, 0.0f, 0.0f} };

	std::vector<Cube> cubes;
	cubes.reserve(100);
	
	for (int i = 0; i < 20; i++) {
		cubes.push_back({
			// Front face
			{-1.0f, -1.0f, -1.0f}, // a
			{ 1.0f, -1.0f, -1.0f}, // b
			{ 1.0f,  1.0f, -1.0f}, // c
			{-1.0f,  1.0f, -1.0f}, // d

			// Back face
			{-1.0f, -1.0f,  1.0f}, // a1
			{ 1.0f, -1.0f,  1.0f}, // b1
			{ 1.0f,  1.0f,  1.0f}, // c1
			{-1.0f,  1.0f,  1.0f}, // d1

			// Transform
			{500.0f * i, 500.0f, 1000.0f}, // pos
			{400.0f, 400.0f, 200.0f}, // size
			{0.0f, 0.0f, 0.0f}  // rotate
		});
	}


	for (int i = 0; i < 20; i++) {
		cubes.push_back({
			// Front face
			{-1.0f, -1.0f, -1.0f}, // a
			{ 1.0f, -1.0f, -1.0f}, // b
			{ 1.0f,  1.0f, -1.0f}, // c
			{-1.0f,  1.0f, -1.0f}, // d

			// Back face
			{-1.0f, -1.0f,  1.0f}, // a1
			{ 1.0f, -1.0f,  1.0f}, // b1
			{ 1.0f,  1.0f,  1.0f}, // c1
			{-1.0f,  1.0f,  1.0f}, // d1

			// Transform
			{500.0f * i, 500.0f, -1000.0f}, // pos
			{400.0f, 400.0f, 200.0f}, // size
			{0.0f, 0.0f, 0.0f}  // rotate
			});
	}

	for (int i = 0; i < 20; i++) {
		cubes.push_back({
			// Front face
			{-1.0f, -1.0f, -1.0f}, // a
			{ 1.0f, -1.0f, -1.0f}, // b
			{ 1.0f,  1.0f, -1.0f}, // c
			{-1.0f,  1.0f, -1.0f}, // d

			// Back face
			{-1.0f, -1.0f,  1.0f}, // a1
			{ 1.0f, -1.0f,  1.0f}, // b1
			{ 1.0f,  1.0f,  1.0f}, // c1
			{-1.0f,  1.0f,  1.0f}, // d1

			// Transform
				{500.0f * i, -500.0f, -400.0f}, // pos
				{400.0f, 800.0f, 100.0f}, // size
				{-45.0f, 0.0f, 0.0f}  // rotate
			});
	}

	for (int i = 0; i < 20; i++) {
		cubes.push_back({
			// Front face
			{-1.0f, -1.0f, -1.0f}, // a
			{ 1.0f, -1.0f, -1.0f}, // b
			{ 1.0f,  1.0f, -1.0f}, // c
			{-1.0f,  1.0f, -1.0f}, // d

			// Back face
			{-1.0f, -1.0f,  1.0f}, // a1
			{ 1.0f, -1.0f,  1.0f}, // b1
			{ 1.0f,  1.0f,  1.0f}, // c1
			{-1.0f,  1.0f,  1.0f}, // d1

			// Transform
				{500.0f * i, -500.0f, 400.0f}, // pos
				{400.0f, 800.0f, 100.0f}, // size
				{45.0f, 0.0f, 0.0f}  // rotate
			});
	}

	cubes.push_back({
		// Front face
		{-1.0f, -1.0f, -1.0f}, // a
		{ 1.0f, -1.0f, -1.0f}, // b
		{ 1.0f,  1.0f, -1.0f}, // c
		{-1.0f,  1.0f, -1.0f}, // d

		// Back face
		{-1.0f, -1.0f,  1.0f}, // a1
		{ 1.0f, -1.0f,  1.0f}, // b1
		{ 1.0f,  1.0f,  1.0f}, // c1
		{-1.0f,  1.0f,  1.0f}, // d1

		// Transform
		{-500.0f, 500.0f, 650.0f}, // pos
		{400.0f, 400.0f, 100.0f}, // size
		{0.0f, 90.0f, 0.0f}  // rotate
		});

	cubes.push_back({
		// Front face
		{-1.0f, -1.0f, -1.0f}, // a
		{ 1.0f, -1.0f, -1.0f}, // b
		{ 1.0f,  1.0f, -1.0f}, // c
		{-1.0f,  1.0f, -1.0f}, // d

		// Back face
		{-1.0f, -1.0f,  1.0f}, // a1
		{ 1.0f, -1.0f,  1.0f}, // b1
		{ 1.0f,  1.0f,  1.0f}, // c1
		{-1.0f,  1.0f,  1.0f}, // d1

		// Transform
		{-500.0f, 500.0f, -650.0f}, // pos
		{400.0f, 400.0f, 100.0f}, // size
		{0.0f, 90.0f, 0.0f}  // rotate
	});

	Cube door = {
		// Front face
		{-1.0f, -1.0f, -1.0f}, // a
		{ 1.0f, -1.0f, -1.0f}, // b
		{ 1.0f,  1.0f, -1.0f}, // c
		{-1.0f,  1.0f, -1.0f}, // d

		// Back face
		{-1.0f, -1.0f,  1.0f}, // a1
		{ 1.0f, -1.0f,  1.0f}, // b1
		{ 1.0f,  1.0f,  1.0f}, // c1
		{-1.0f,  1.0f,  1.0f}, // d1

		// Transform
		{-250.0f, 500.0f, 250.0f}, // pos
		{200.0f, 400.0f, 100.0f}, // size
		{0.0f, -90.0f, 0.0f},  // rotate
		{-1.0f, 0.0f, 0.0f},
	};

	cubes.push_back(door);

	cubes.push_back({
		// Front face
		{-1.0f, -1.0f, -1.0f}, // a
		{ 1.0f, -1.0f, -1.0f}, // b
		{ 1.0f,  1.0f, -1.0f}, // c
		{-1.0f,  1.0f, -1.0f}, // d

		// Back face
		{-1.0f, -1.0f,  1.0f}, // a1
		{ 1.0f, -1.0f,  1.0f}, // b1
		{ 1.0f,  1.0f,  1.0f}, // c1
		{-1.0f,  1.0f,  1.0f}, // d1

		// Transform
		{500.0f * 20, 500.0f, 0.0f}, // pos
		{1200.0f, 400.0f, 200.0f}, // size
		{0.0f, 90.0f, 0.0f}  // rotate
	});

	Camera camera{};

	camera.pos.y += 200.0f;

	bool running = true;
	bool isCameraModeOn = false;

	constexpr int doorIndex = 82;

	Vector2 previousMouse;

	SDL_SetWindowRelativeMouseMode(window, true);

	constexpr int jobsSize = 1000;
	std::future<void> jobs[jobsSize];
	int jobIndex = 0;

	double time = GetTime();

	constexpr float renderingFixedDelta = 1.0f / 60.0f;

	bool animateDoor = false;
	int doorAnimationDir = 1;

	while (running) {
		jobIndex = 0;
		std::fill(frameBuffer, frameBuffer + config.WIDTH * config.HEIGHT, 0u);
		std::fill(depthBuffer, depthBuffer + config.WIDTH * config.HEIGHT, 1e5);

		double now = GetTime();
		double delta = now - time;

		if (delta < renderingFixedDelta) {
			continue;
		}

		delta -= renderingFixedDelta;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_MOUSE_MOTION)
			{
				constexpr float sensitivity = 0.1f;

				camera.rotate.y -= event.motion.xrel * sensitivity;
				camera.rotate.x += event.motion.yrel * sensitivity;

				if (camera.rotate.x > 89.0f)
					camera.rotate.x = 89.0f;

				if (camera.rotate.x < -89.0f)
					camera.rotate.x = -89.0f;
			}

			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (event.key.scancode == SDL_SCANCODE_ESCAPE)
					running = false;

				if (event.key.scancode == SDL_SCANCODE_E) {
					animateDoor = true;
					doorAnimationDir *= (- 1);
				}
			}

			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}

		const bool* keys = SDL_GetKeyboardState(nullptr);

		constexpr float moveSpeed = 2000.0f;

		float yaw = camera.rotate.y * std::numbers::pi_v<float> / 180.0f;
		float pitch = camera.rotate.x * std::numbers::pi_v<float> / 180.0f;

		Vector3 forward = {
			-std::sin(yaw),
			0.0f,
			 std::cos(yaw)
		};

		Vector3 right = {
			std::cos(yaw),
			0.0f,
			std::sin(yaw)
		};

		if (keys[SDL_SCANCODE_W])
		{
			camera.pos.x += forward.x * moveSpeed * renderingFixedDelta;
			camera.pos.y += forward.y * moveSpeed * renderingFixedDelta;
			camera.pos.z += forward.z * moveSpeed * renderingFixedDelta;
		}

		if (keys[SDL_SCANCODE_S])
		{
			camera.pos.x -= forward.x * moveSpeed * renderingFixedDelta;
			camera.pos.y -= forward.y * moveSpeed * renderingFixedDelta;
			camera.pos.z -= forward.z * moveSpeed * renderingFixedDelta;
		}

		if (keys[SDL_SCANCODE_D])
		{
			camera.pos.x += right.x * moveSpeed * renderingFixedDelta;
			camera.pos.y += right.y * moveSpeed * renderingFixedDelta;
			camera.pos.z += right.z * moveSpeed * renderingFixedDelta;
		}

		if (keys[SDL_SCANCODE_A])
		{
			camera.pos.x -= right.x * moveSpeed * renderingFixedDelta;
			camera.pos.y -= right.y * moveSpeed * renderingFixedDelta;
			camera.pos.z -= right.z * moveSpeed * renderingFixedDelta;
		}

		//Renderer2D::DrawQuad(quad, frameBuffer, config, gorillaTexture);

		//Renderer3D::DrawTethradon(tethradon, frameBuffer, depthBuffer, config);

		if (animateDoor) {
			cubes[doorIndex].rotate.y += doorAnimationDir * 10.0f * renderingFixedDelta;
		}

		for (auto& cube : cubes) {
			jobs[jobIndex] = (ThreadPool::GetInstance().SubmitJob(
				[&cube, &frameBuffer, &depthBuffer, &config, &camera] {
					Renderer3D::DrawCube(
						cube,
						frameBuffer,
						depthBuffer,
						config,
						camera
					);
				}
			));

			jobIndex++;
		}

		for (int i = 0; i < jobsSize; i++) {
			if (jobs[i]._Ptr() != nullptr) {
				jobs[i].wait();
			}
		}

		//Renderer3D::DrawCube(cubeMesh, frameBuffer, depthBuffer, config, camera, zombieTexture);

		//DrawCircle(Circle2D::Create({ config.WIDTH * 0.5f, config.HEIGHT * 0.5f }, { 100.0f, 100.0f }), frameBuffer, config);

		SDL_UpdateTexture(texture, nullptr, frameBuffer, config.WIDTH * sizeof(uint32_t));

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		time = now;
	}

	SDL_Quit();

	delete[] frameBuffer;
	frameBuffer = nullptr;
	delete[] depthBuffer;
	depthBuffer = nullptr;

	return 0;
}
