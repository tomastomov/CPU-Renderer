#include "pch.h";
#include <Log.h>
#include <SDL3/SDL.h>

static uint32_t GetColorFromARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

struct Vector2 {
    float x;
    float y;
    
    double GetLength() {
        double xSquared = x * x;
        double ySquared = y * y;

        return std::sqrt(xSquared + ySquared);
    }

    Vector2 GetNormalized() {
        double length = GetLength();

        if (length <= 0.01) {
            CPURenderer::Log("Length of a vector is too small skipping normalization");
            return *this;
        }

        return Vector2(x / length, y / length);
    }

    Vector2 operator-(const Vector2 other) {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(const float value) {
        return Vector2(x * value, y * value);
    }
};

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

        if (startX >= WIDTH || startY >= HEIGHT) {
            return;
        }

        int frameBufferIndex = std::round(startY) * WIDTH + std::round(startX);
        int all = WIDTH * HEIGHT;

        frameBuffer[all - frameBufferIndex] = GetColorFromARGB(255, 0, 0, 255);

        xCondition = normalizedLine.x >= 0.0f ? startX <= endX : startX >= endX;
        yCondition = normalizedLine.y >= 0.0f ? startY <= endY : startY >= endY;
        CPURenderer::Log("Drawing pixel point x: {}, y: {} at screen location {}", startX, startY, frameBufferIndex);
    }
}

static void DrawTriangle(Vector2 a, Vector2 b, Vector2 c, uint32_t* frameBuffer, int WIDTH, int HEIGHT) {
    double aLen = a.GetLength();
    double bLen = b.GetLength();
    double cLen = c.GetLength();

    if (aLen + bLen <= cLen || aLen + cLen <= bLen || bLen + cLen <= aLen) {
        CPURenderer::Log("Invalid triangle");
        return;
    }

    DrawLine(a, b, frameBuffer, WIDTH, HEIGHT);
    DrawLine(a, c, frameBuffer, WIDTH, HEIGHT);
    DrawLine(b, c, frameBuffer, WIDTH, HEIGHT);

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

    uint32_t* frameBuffer = new uint32_t[WIDTH * HEIGHT];
    
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (i > WIDTH * 50) {
            frameBuffer[i] = GetColorFromARGB(0, 0, 0, 0);
        }
        else {
            frameBuffer[i] = GetColorFromARGB(255, 255, 0, 0);
        }
    }

    Vector2 v1 = { 20.0f, 20.0f };
    Vector2 v2 = { 40.0f, 20.0f };
    Vector2 v3 = { 30.0f, 60.0f };

    DrawTriangle(v1, v2, v3, frameBuffer, WIDTH, HEIGHT);
    DrawTriangle(v1 * 10.0f, v2 * 10.0f, v3 * 10.0f, frameBuffer, WIDTH, HEIGHT);
    DrawTriangle({(WIDTH * 0.5f) + 200.0f, (HEIGHT * 0.5f) - 200.0f }, {(WIDTH * 0.5f - 200.0f), (HEIGHT * 0.5f) - 200.0f}, { WIDTH * 0.5f, HEIGHT - 200.0f }, frameBuffer, WIDTH, HEIGHT);

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
