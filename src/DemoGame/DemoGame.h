#pragma once

#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Engine/Graphics/Texture.h"
#include <SDL_events.h>
#include <SDL_timer.h>
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Backgrounds.h"
#include "DemoGame/Tilemap.h"

// Componente personalizado para el jugador
struct PlayerComponent {};

// Sistema para generar el jugador
class SquareSpawnSetupSystem : public SetupSystem {
  void run() override {
    Entity* square = scene->createEntity("SQUARE", 10, 10); 
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300); // Velocidad base
    square->addComponent<TextureComponent>("src/images/idle.png");
    square->addComponent<SpriteComponent>("src/images/idle.png", 64, 64, 1, 4, 1000); // Sprite animado
    square->addComponent<BoxColliderComponent>(
        SDL_Rect{0, 0, 64, 64}, // Rectángulo del colisionador
        SDL_Color{255, 0, 0}    // Color del colisionador
    );
  }
};

// Sistema para configurar la cámara
class CameraSetupSystem : public SetupSystem {
  void run() override {
    int width = 400; // Ancho de la ventana
    int height = 600; // Alto de la ventana

    scene->mainCamera = scene->createEntity("CAMERA", 0, 0);
    scene->mainCamera->addComponent<CameraComponent>(
        1,          // Zoom inicial
        width,      // Ancho de la ventana
        height,     // Alto de la ventana
        width * 10, // Ancho total del mundo
        height * 10 // Alto total del mundo
    );
    scene->mainCamera->addComponent<PositionComponent>(0, 0); // Posición inicial de la cámara
  }
};

// Sistema para que la cámara siga al jugador
class CameraFollowUpdateSystem : public UpdateSystem {
  void run(float dT) override {
    auto view = scene->r.view<PlayerComponent, PositionComponent, SpriteComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto& cameraComponent = scene->mainCamera->get<CameraComponent>();

    for (auto e : view) {
      auto& playerPos = view.get<PositionComponent>(e);
      auto& playerSpr = view.get<SpriteComponent>(e);

      // Calcular el tamaño del sprite con el zoom actual
      int spriteWidth = static_cast<int>(playerSpr.width * playerSpr.scale * cameraComponent.zoom);
      int spriteHeight = static_cast<int>(playerSpr.height * playerSpr.scale * cameraComponent.zoom);

      // Calcular nueva posición de la cámara centrada en el jugador
      int px = playerPos.x - cameraComponent.vw / 2 + spriteWidth / 2;
      int py = playerPos.y - cameraComponent.vh / 2 + spriteHeight / 2;

      // Restringir la posición de la cámara dentro de los límites del mundo
      if (px > 0 && px < cameraComponent.ww - cameraComponent.vw) {
        cameraPosition.x = px;
      }

      if (py > 0 && py < cameraComponent.wh - cameraComponent.vh) {
        cameraPosition.y = py;
      }
    }
  }
};

// Sistema de movimiento
class MovementSystem : public UpdateSystem {
  void run(float dT) override {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);

      pos.x += vel.x * dT;
      pos.y += vel.y * dT;
    }
  }
};

// Sistema para manejar entradas de movimiento
class MovementInputSystem : public EventSystem {
  void run(SDL_Event event) override {
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

// Clase principal del juego
class DemoGame : public Game {
public:
  Scene* sampleScene;
  entt::registry r;

public:
  DemoGame()
    : Game("SAMPLE", 400, 600) // Dimensiones de la ventana del juego
  { }

  void setup() override { 
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);

    // Sistemas de configuración
    addSetupSystem<CameraSetupSystem>(sampleScene);
    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<TilemapSetupSystem>(sampleScene);

    // Sistemas de eventos
    addEventSystem<MovementInputSystem>(sampleScene);

    // Sistemas de actualización
    addUpdateSystem<CameraFollowUpdateSystem>(sampleScene);
    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<SpriteAnimationSystem>(sampleScene);

    // Sistemas de renderizado
    addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<TilemapRenderSystem>(sampleScene);

    setScene(sampleScene); // Establecer la escena actual
  }
};
