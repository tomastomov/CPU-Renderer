#include "pch.h";
#include <Log.h>
#include <SDL3/SDL.h>

static uint32_t GetColorFromARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
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

    uint32_t* frameBuffer = new uint32_t[WIDTH * HEIGHT];
    
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (i > WIDTH * 50) {
            frameBuffer[i] = GetColorFromARGB(0, 0, 0, 0);
        }
        else {
            frameBuffer[i] = GetColorFromARGB(255, 255, 0, 0);
        }
    }

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

    return 0;
}
