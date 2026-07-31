#include "pch.h"
#include <Log.h>
#include <SDL3/SDL.h>
#include <Vector2.h>

using CPURenderer::Vector2;

//TODO::fix projection matrix and rotation

struct Matrix3x3 {
    float arr[3][3];
    Matrix3x3(const int(&a)[3][3]) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                arr[i][j] = a[i][j];
            }
        }
    }

    static Matrix3x3 GetIdentity() {
        static constexpr int identity[3][3] = {
            {1, 0, 0,}, {0, 1, 0}, {0, 0, 1}
        };

        return Matrix3x3(identity);
    }

    void Scale(const float scaleFactor) {
        for (int i = 0; i < 2; i++) {
            arr[i][i] *= scaleFactor;
        }
    }

    void Scale(const Vector2 v) {
        arr[0][0] *= v.x;
        arr[1][1] *= v.y;
    }

    void Rotate(const float angle) {
        float radians = angle * std::numbers::pi / 180.0f;
        float cos = std::cos(radians);
        float sin = std::sin(radians);

        arr[0][0] = cos;
        arr[0][1] = -sin;
        arr[1][0] = sin;
        arr[1][1] = cos;
    }

    void Translate(const Vector2 v) {
        for (int i = 0; i < 2; i++) {
            arr[i][2] = (i == 0 ? v.x : v.y);
        }
    }

    void Print() {
        CPURenderer::Log("First row: {} {} {}", arr[0][0], arr[0][1], arr[0][2]);
        CPURenderer::Log("Second row: {} {} {}", arr[1][0], arr[1][1], arr[1][2]);
        CPURenderer::Log("Third row: {} {} {}", arr[2][0], arr[2][1], arr[2][2]);
    }

    Matrix3x3 operator *(const Matrix3x3& other) {
        int result[3][3];

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result[i][j] = (arr[i][0] * other.arr[0][j]) + (arr[i][1] * other.arr[1][j]) + (arr[i][2] * other.arr[2][j]);
            }
        }

        return Matrix3x3(result);
    }

    Vector2 operator *(const Vector2 v) {
        Vector2 res;

        res.x = (arr[0][0] * v.x) + (arr[0][1] * v.y) + (arr[0][2] * 1);
        res.y = (arr[1][0] * v.x) + (arr[1][1] * v.y) + (arr[1][2] * 1);

        return res;
    }
};

struct Triangle2D {
    Vector2 a;
    Vector2 b;
    Vector2 c;
    Vector2 pos;
    Vector2 size;
    float rotate;
    
    static Triangle2D Create(Vector2 a, Vector2 b, Vector2 c, Vector2 pos, Vector2 size, float rotate) {
        return Triangle2D(a, b, c, pos, size, rotate);
    }
};

static uint32_t GetColorFromARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

static void DrawLine(Vector2 a, Vector2 b, uint32_t* frameBuffer, const int WIDTH, const int HEIGHT) {
    //CPURenderer::Log("Processing point x: {}, y: {} and x1: {}, y1: {} ", a.x, a.y, b.x, b.y);
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

        if (std::round(startX) >= WIDTH || std::round(startY) >= HEIGHT) {
            return;
        }
        else if (startX < 0 || startY < 0) {
            return;
        }

        int frameBufferIndex = std::round(startY) * WIDTH + std::round(startX);
        
        int all = WIDTH * HEIGHT;

        frameBuffer[all - 1 - frameBufferIndex] = GetColorFromARGB(255, 0, 0, 255);

        xCondition = normalizedLine.x >= 0.0f ? startX <= endX : startX >= endX;
        yCondition = normalizedLine.y >= 0.0f ? startY <= endY : startY >= endY;
        //CPURenderer::Log("Drawing pixel point x: {}, y: {} at screen location {}", startX, startY, frameBufferIndex);
    }
}

static void DrawTriangle(Triangle2D& triangle, uint32_t* frameBuffer, int WIDTH, int HEIGHT) {
    double abLen = (triangle.b - triangle.a).GetLength();
    double bcLen = (triangle.c - triangle.b).GetLength();
    double caLen = (triangle.a - triangle.c).GetLength();

    if (abLen + bcLen <= caLen ||
        abLen + caLen <= bcLen ||
        bcLen + caLen <= abLen)
    {
        CPURenderer::Log("Invalid triangle");
        return;
    }

    auto model = Matrix3x3::GetIdentity();
    auto identityScale = Matrix3x3::GetIdentity();
    identityScale.Scale(triangle.size);
   
    auto identityMove = Matrix3x3::GetIdentity();
    identityMove.Translate(triangle.pos);

    Matrix3x3 projectionMatrix = Matrix3x3::GetIdentity();
    projectionMatrix.Translate({ 1 + (WIDTH * 0.5f), 1 + (HEIGHT * 0.5f) });

    model = identityMove * identityScale;

    triangle.a = projectionMatrix * model * triangle.a;
    triangle.b = projectionMatrix * model * triangle.b;
    triangle.c = projectionMatrix * model * triangle.c;

    Vector2 caLine = (triangle.c - triangle.a).GetNormalized();

    Vector2 cbLine = (triangle.c - triangle.b).GetNormalized();
    
    while (triangle.a.x * (caLine.x >= 0.0f ? 1.0f : -1.0f) <= triangle.c.x && triangle.a.y * (caLine.y >= 0.0f ? 1.0f : -1.0f) <= triangle.c.y && triangle.b.x * (cbLine.x >= 0.0f ? 1.0f : -1.0f) <= triangle.c.x && triangle.b.y * (cbLine.y >= 0.0f ? 1.0f : -1.0f) <= triangle.c.y) {
        triangle.a.x += caLine.x;
        triangle.a.y += caLine.y;

        triangle.b.x += cbLine.x;
        triangle.b.y += cbLine.y;

        DrawLine(triangle.a, triangle.b, frameBuffer, WIDTH, HEIGHT);
    }

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

    Vector2 bottomLeft = { -0.5f, -0.5f };
    Vector2 topLeft = { -0.5f, 0.5f };
    Vector2 bottomRight = { 0.5f, -0.5f };

    Triangle2D triangle = Triangle2D::Create(bottomLeft, bottomRight, topLeft, {0.0f, 0.0f}, {100.0f, 100.0f}, 90.0f);

    DrawTriangle(triangle, frameBuffer, WIDTH, HEIGHT);

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

    delete frameBuffer;
    frameBuffer = nullptr;

    return 0;
}
