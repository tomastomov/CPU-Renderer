#include "pch.h";
#include <Log.h>
#include <SDL3/SDL.h>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        CPURenderer::Log("Failed to init sdl video");
        return -1;
    }

    CPURenderer::Log("SDL init successfully");

    constexpr int WIDTH = 1920;
    constexpr int HEIGHT = 1080;

    SDL_Window* window = SDL_CreateWindow("CPU Renderer", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);

    if (!window) {
        CPURenderer::Log("Failed to create window");
        return -1;
    }

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
    }

    SDL_Quit();

    return 0;
}
