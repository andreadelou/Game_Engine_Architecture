#include "ECS.h"

void ECS::init(SDL_Renderer* renderer) {
    renderSystem.init(renderer);
    movementSystem.init();
    collisionSystem.init();

    // Crear paddle
    auto paddle = registry.create();
    registry.emplace<Position>(paddle, 270.0f, 440.0f);
    registry.emplace<Velocity>(paddle, 0.0f, 0.0f);
    registry.emplace<Size>(paddle, 100.0f, 20.0f);
    registry.emplace<Color>(paddle, 0x00, 0x00, 0xFF, 0xFF);
    registry.emplace<Paddle>(paddle);

    // Crear bola
    auto ball = registry.create();
    registry.emplace<Position>(ball, 300.0f, 300.0f);
    registry.emplace<Velocity>(ball, 100.0f, 100.0f);
    registry.emplace<Size>(ball, 10.0f, 10.0f);
    registry.emplace<Color>(ball, 0xFF, 0x00, 0x00, 0xFF);
    registry.emplace<Ball>(ball);

    // Crear bloques
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 10; ++x) {
            auto block = registry.create();
            registry.emplace<Position>(block, x * 60.0f, y * 20.0f);
            registry.emplace<Size>(block, 60.0f, 20.0f);
            registry.emplace<Color>(block, 0x00, 0xFF, 0x00, 0xFF);
            registry.emplace<Block>(block);
        }
    }
}

void ECS::update(float dT) {
    movementSystem.update(registry, dT);
    collisionSystem.update(registry);
}

void ECS::render(SDL_Renderer* renderer) {
    renderSystem.update(registry, renderer);
}
