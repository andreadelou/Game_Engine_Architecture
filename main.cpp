#include <SDL_keyboard.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <string>
#include <vector>
#include <iostream>
#include <SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const int BALL_SPEED = 100;
const int BALL_SIZE = 10;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BLOCK_WIDTH = 60;
const int BLOCK_HEIGHT = 20;

struct Rect {
    SDL_Rect rect;
    int vx;
    int vy;
    SDL_Color color;
};

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};

Rect ball = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, BALL_SIZE, BALL_SIZE}, BALL_SPEED, BALL_SPEED, red};
Rect paddle = {{SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 50, PADDLE_WIDTH, PADDLE_HEIGHT}, 0, 0, blue};
std::vector<Rect> blocks;

void renderRect(SDL_Renderer* renderer, Rect& rect) {
    SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
    SDL_RenderFillRect(renderer, &rect.rect);
}

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
    );
}

void handleInput(SDL_Event& e) {
    const Uint8* ks = SDL_GetKeyboardState(NULL);
    paddle.vx = 0;

    if (ks[SDL_SCANCODE_LEFT]) {
        paddle.vx = -BALL_SPEED;
    }
    if (ks[SDL_SCANCODE_RIGHT]) {
        paddle.vx = BALL_SPEED;
    }
}

void update(float dT) {
    paddle.rect.x += paddle.vx * dT;
    if (paddle.rect.x < 0) {
        paddle.rect.x = 0;
    }
    if (paddle.rect.x + PADDLE_WIDTH > SCREEN_WIDTH) {
        paddle.rect.x = SCREEN_WIDTH - PADDLE_WIDTH;
    }

    ball.rect.x += ball.vx * dT;
    ball.rect.y += ball.vy * dT;

    if (ball.rect.x < 0 || ball.rect.x + BALL_SIZE > SCREEN_WIDTH) {
        ball.vx *= -1;
    }
    if (ball.rect.y < 0) {
        ball.vy *= -1;
    }

    if (ball.rect.y + BALL_SIZE > SCREEN_HEIGHT) {
        std::cout << "Game Over" << std::endl;
        SDL_Quit();
        exit(0);
    }

    if (checkCollision(ball.rect, paddle.rect)) {
        ball.vy *= -1;
        ball.vx += (rand() % 5 - 2) * 10;  // Añadir variación aleatoria a la velocidad X de la pelota
    }

    for (auto it = blocks.begin(); it != blocks.end(); ) {
        if (checkCollision(ball.rect, it->rect)) {
            ball.vy *= -1;
            it = blocks.erase(it);
            if (blocks.empty()) {
                std::cout << "You Win!" << std::endl;
                SDL_Quit();
                exit(0);
            }
        } else {
            ++it;
        }
    }
}

void createBlocks() {
    int rows = 5;
    int cols = SCREEN_WIDTH / BLOCK_WIDTH;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Rect block = {{x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT}, 0, 0, green};
            blocks.push_back(block);
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    createBlocks();

    bool quit = false;
    SDL_Event e;

    Uint32 frameCount = 0; // Se declara la variable frameCount aquí
    Uint32 frameStartTimestamp;
    Uint32 frameEndTimestamp;
    Uint32 lastFrameTime = SDL_GetTicks();
    Uint32 lastUpdateTime = 0;
    float frameDuration = (1.0 / MAX_FPS) * 1000.0;
    float actualFrameDuration;
    int FPS = MAX_FPS;

    while (!quit) {
        frameStartTimestamp = SDL_GetTicks();

        Uint32 currentFrameTime = SDL_GetTicks();
        float dT = (currentFrameTime - lastFrameTime) / 1000.0;
        lastFrameTime = currentFrameTime;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handleInput(e);
        }

        update(dT);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        renderRect(renderer, paddle);
        renderRect(renderer, ball);
        for (auto& block : blocks) {
            renderRect(renderer, block);
        }

        SDL_RenderPresent(renderer);

        frameEndTimestamp = SDL_GetTicks();
        actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

        if (actualFrameDuration < frameDuration) {
            SDL_Delay(frameDuration - actualFrameDuration);
        }

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - lastUpdateTime;
        if (elapsedTime > 1000) {
            FPS = (float)frameCount / (elapsedTime / 1000.0);
            lastUpdateTime = currentTime;
            frameCount = 0;
        }
        SDL_SetWindowTitle(window, ("FPS: " + std::to_string(FPS)).c_str());
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
