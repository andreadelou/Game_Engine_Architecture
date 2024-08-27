#pragma once
#include <entt/entt.hpp>
#include <SDL.h>
#include "Components.h"

class RenderSystem {
public:
    void init(SDL_Renderer* renderer) {}
    void update(entt::registry& registry, SDL_Renderer* renderer);
};

class MovementSystem {
public:
    void init() {}
    void update(entt::registry& registry, float dT);
};

class CollisionSystem {
public:
    void init() {}
    void update(entt::registry& registry);

private:
    bool checkCollision(const Position& a, const Size& aSize, const Position& b, const Size& bSize);
};
