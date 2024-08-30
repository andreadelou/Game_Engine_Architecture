#include <SDL_keyboard.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <string>
#include <vector>
#include <iostream>
#include <SDL.h>
#include <entt/entt.hpp>

// Componente para posición y tamaño
struct TransformComponent {
    SDL_Rect rect;
};

// Componente para velocidad
struct VelocityComponent {
    int vx, vy;
};

// Componente para color
struct ColorComponent {
    SDL_Color color;
};

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const int BALL_SPEED = 100;
const int BALL_SIZE = 10;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BLOCK_WIDTH = 60;
const int BLOCK_HEIGHT = 20;

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};

void renderRect(SDL_Renderer* renderer, TransformComponent& transform, ColorComponent& color) {
    SDL_SetRenderDrawColor(renderer, color.color.r, color.color.g, color.color.b, color.color.a);
    SDL_RenderFillRect(renderer, &transform.rect);
}

void render(SDL_Renderer* renderer, entt::registry& registry) {
    auto view = registry.view<TransformComponent, ColorComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& color = view.get<ColorComponent>(entity);

        renderRect(renderer, transform, color);
    }
}

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
    );
}

void handleInput(SDL_Event& e, entt::registry& registry, entt::entity paddleEntity) {
    const Uint8* ks = SDL_GetKeyboardState(NULL);

    auto& paddleVelocity = registry.get<VelocityComponent>(paddleEntity);
    paddleVelocity.vx = 0;

    if (ks[SDL_SCANCODE_LEFT]) {
        paddleVelocity.vx = -BALL_SPEED;
    }
    if (ks[SDL_SCANCODE_RIGHT]) {
        paddleVelocity.vx = BALL_SPEED;
    }
}

void update(float dT, entt::registry& registry, entt::entity ballEntity, entt::entity paddleEntity) {
    auto view = registry.view<TransformComponent, VelocityComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& velocity = view.get<VelocityComponent>(entity);

        // Actualiza la posición de la entidad
        transform.rect.x += velocity.vx * dT;
        transform.rect.y += velocity.vy * dT;

        // Reglas de colisión para la pelota
        if (entity == ballEntity) {
            if (transform.rect.x < 0 || transform.rect.x + transform.rect.w > SCREEN_WIDTH) {
                velocity.vx *= -1;
            }
            if (transform.rect.y < 0) {
                velocity.vy *= -1;
            }
            if (transform.rect.y + transform.rect.h > SCREEN_HEIGHT) {
                std::cout << "Game Over" << std::endl;
                SDL_Quit();
                exit(0);
            }

            // Colisión con el paddle
            auto& paddleTransform = registry.get<TransformComponent>(paddleEntity);
            if (checkCollision(transform.rect, paddleTransform.rect)) {
                velocity.vy *= -1;
                velocity.vx += (rand() % 5 - 2) * 10; // Añadir variación aleatoria a la velocidad X de la pelota
            }
        }
    }

    // Colisión con bloques
    auto blockView = registry.view<TransformComponent, ColorComponent>();
    for (auto entity : blockView) {
        if (entity != ballEntity && entity != paddleEntity) {
            auto& blockTransform = blockView.get<TransformComponent>(entity);
            if (checkCollision(registry.get<TransformComponent>(ballEntity).rect, blockTransform.rect)) {
                registry.get<VelocityComponent>(ballEntity).vy *= -1;
                registry.destroy(entity); // Destruye el bloque
            }
        }
    }

    // Verifica si todos los bloques han sido destruidos
    if (blockView.begin() == blockView.end()) {
        std::cout << "You Win!" << std::endl;
        SDL_Quit();
        exit(0);
    }
}


void createBlocks(entt::registry& registry) {
    int rows = 5;
    int cols = SCREEN_WIDTH / BLOCK_WIDTH;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            auto blockEntity = registry.create();
            registry.emplace<TransformComponent>(blockEntity, SDL_Rect{x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT});
            registry.emplace<ColorComponent>(blockEntity, green);
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    entt::registry registry;

    // Crear la entidad para la pelota
    auto ballEntity = registry.create();
    registry.emplace<TransformComponent>(ballEntity, SDL_Rect{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, BALL_SIZE, BALL_SIZE});
    registry.emplace<VelocityComponent>(ballEntity, BALL_SPEED, BALL_SPEED);
    registry.emplace<ColorComponent>(ballEntity, red);

    // Crear la entidad para el paddle
    auto paddleEntity = registry.create();
    registry.emplace<TransformComponent>(paddleEntity, SDL_Rect{SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 50, PADDLE_WIDTH, PADDLE_HEIGHT});
    registry.emplace<VelocityComponent>(paddleEntity, 0, 0);
    registry.emplace<ColorComponent>(paddleEntity, blue);

    // Crear las entidades para los bloques
    createBlocks(registry);

    // El bucle principal se mantendrá igual
    bool quit = false;
    SDL_Event e;

    Uint32 frameCount = 0;
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
            handleInput(e, registry, paddleEntity);
        }

        update(dT, registry, ballEntity, paddleEntity);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        render(renderer, registry);

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
