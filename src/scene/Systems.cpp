#include "Systems.h"

void RenderSystem::update(entt::registry& registry, SDL_Renderer* renderer) {
    registry.view<Position, Size, Color>().each([&](auto entity, auto& pos, auto& size, auto& color) {
        SDL_Rect rect = { static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(size.w), static_cast<int>(size.h) };
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    });
}

void MovementSystem::update(entt::registry& registry, float dT) {
    registry.view<Position, Velocity>().each([&](auto entity, auto& pos, auto& vel) {
        pos.x += vel.vx * dT;
        pos.y += vel.vy * dT;
    });
}

void CollisionSystem::update(entt::registry& registry) {
    auto view = registry.view<Position, Size, Velocity>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& size = view.get<Size>(entity);
        auto& vel = view.get<Velocity>(entity);

        if (pos.x < 0 || pos.x + size.w > 640) {
            vel.vx *= -1;
        }
        if (pos.y < 0 || pos.y + size.h > 480) {
            vel.vy *= -1;
        }
    }
}

bool CollisionSystem::checkCollision(const Position& a, const Size& aSize, const Position& b, const Size& bSize) {
    return (
        a.x < b.x + bSize.w &&
        a.x + aSize.w > b.x &&
        a.y < b.y + bSize.h &&
        a.y + aSize.h > b.y
    );
}
