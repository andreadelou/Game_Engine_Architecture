#include "DemoGame/Tilemap.h"
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <SDL_events.h>
#include <SDL_timer.h>
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Backgrounds.h"
#include "Player.h"
#include "Colliders.h"


class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    // Crear la entidad "SQUARE"
    Entity* square = scene->createEntity("SQUARE", 10, 10);
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("src/images/idle.png");
    square->addComponent<SpriteComponent>("src/images/idle.png", 64, 64, 1, 4, 1000);
    square->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 64, 64}, SDL_Color{255, 0, 0});

    // Crear la entidad "face"
    // Entity* face = scene->createEntity("face", 200, 200);
    // face->addComponent<PowerUpComponent>();
    // face->addComponent<TextureComponent>("src/images/face.png");
    // face->addComponent<SpriteComponent>("src/images/face.png", 64, 64, 1, 4, 1000);
    // face->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 64, 64}, SDL_Color{0, 255, 0});
  }
};

class MovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();
    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      pos.x += vel.x * dT;
      pos.y += vel.y * dT;
    }
  }
};

class MovementInputSystem : public EventSystem {
  void run(SDL_Event event) {
    auto view = scene->r.view<PlayerComponent, VelocityComponent>();
    for (auto e : view) {
      auto& vel = view.get<VelocityComponent>(e);

      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
          vel.x = -vel.speed;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          vel.x = vel.speed;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          vel.y = -vel.speed;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          vel.y = vel.speed;
        }
      } else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
          vel.x = 0;
        }
        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
          vel.y = 0;
        }
      }
    }
  }
};



class DemoGame : public Game {
public:
  Scene* sampleScene;
  entt::registry r;

public:
  DemoGame()
      : Game("SAMPLE", 400, 600) {}

  void setup() {
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);

    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<TilemapSetupSystem>(sampleScene);
    addSetupSystem<AdvancedAutoTilingSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);
    addSetupSystem<TilemapEntitySetupSystem>(sampleScene);
    addEventSystem<MovementInputSystem>(sampleScene);

    addUpdateSystem<ColliderResetSystem>(sampleScene);
    addUpdateSystem<SpriteMovementSystem>(sampleScene);

    addUpdateSystem<PlayerPowerUpCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<PlayerPowerUpCollisionSystem>(sampleScene);

    addUpdateSystem<PlayerTileCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<PlayerWallCollisionSystem>(sampleScene);

    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<SpriteAnimationSystem>(sampleScene);
    addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<TilemapRenderSystem>(sampleScene);
    addRenderSystem<ColliderRenderSystem>(sampleScene);

    setScene(sampleScene);
  }
};
