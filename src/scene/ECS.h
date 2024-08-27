#pragma once
#include <entt/entt.hpp>
#include <SDL.h>
#include "Components.h"
#include "Systems.h"

class ECS {
public:
    void init(SDL_Renderer* renderer);
    void update(float dT);
    void render(SDL_Renderer* renderer);

private:
    entt::registry registry;
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    CollisionSystem collisionSystem;
};
