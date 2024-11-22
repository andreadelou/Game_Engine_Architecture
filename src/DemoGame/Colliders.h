#pragma once
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "DemoGame/Tilemap.h"
#include "Player.h"
#include "Colliders.h"
#include <SDL2/SDL.h>
#include <iostream>

// Componente para potenciar al jugador
struct PowerUpComponent {};

// Configuración del jugador
class PlayerSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* player = scene->createEntity("PLAYER", 100, 100);
    player->addComponent<PlayerComponent>();
    player->addComponent<VelocityComponent>(300);
    player->addComponent<PositionComponent>(100, 100);
    player->addComponent<TextureComponent>("assets/Sprites/player.png");
    player->addComponent<SpriteComponent>("assets/Sprites/player.png", 32, 32, 2, 4, 1000);
    player->addComponent<BoxColliderComponent>(
      SDL_Rect{0, 0, 32, 32}, 
      SDL_Color{255, 0, 0, 255}, 
      CollisionType::NONE
    );
  }
};

// Configuración de potenciadores
class PowerUpSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* powerUp = scene->createEntity("POWER_UP", 200, 200);
    powerUp->addComponent<PowerUpComponent>();
    powerUp->addComponent<PositionComponent>(200, 200);
    powerUp->addComponent<TextureComponent>("assets/Sprites/powerup.png");
    powerUp->addComponent<SpriteComponent>("assets/Sprites/powerup.png", 16, 16, 2, 4, 1000);
    powerUp->addComponent<BoxColliderComponent>(
      SDL_Rect{0, 0, 16, 16}, 
      SDL_Color{0, 255, 0, 255}, 
      CollisionType::TRIGGER
    );
  }
};

// Configuración de la cámara
class CameraSetupSystem : public SetupSystem {
public:
  void run() override {
    int width = 800, height = 600;
    scene->mainCamera = scene->createEntity("CAMERA", 0, 0);
    scene->mainCamera->addComponent<CameraComponent>(
      1,      // Zoom inicial
      width,  // Ancho de la cámara
      height, // Alto de la cámara
      2000,   // Ancho del mundo
      2000    // Alto del mundo
    );
    scene->mainCamera->addComponent<PositionComponent>(0, 0);
  }
};

// Clase principal del juego
class MyGame : public Game {
public:
  Scene* gameScene;
  entt::registry r;

  MyGame()
  : Game("My Game", 800, 600) { }

  void setup() override {
    gameScene = new Scene("Main Scene", r, renderer);

    // Sistemas de configuración
    addSetupSystem<PlayerSetupSystem>(gameScene);
    addSetupSystem<PowerUpSetupSystem>(gameScene);
    addSetupSystem<CameraSetupSystem>(gameScene);
    addSetupSystem<TilemapSetupSystem>(gameScene);

    // Sistemas de actualización
    addUpdateSystem<ColliderResetSystem>(gameScene);
    addUpdateSystem<PlayerPowerUpCollisionDetectionSystem>(gameScene);
    addUpdateSystem<PlayerPowerUpCollisionSystem>(gameScene);
    addUpdateSystem<PlayerTileCollisionDetectionSystem>(gameScene);
    addUpdateSystem<PlayerWallCollisionSystem>(gameScene);

    // Sistemas de renderizado
    addRenderSystem<SpriteRenderSystem>(gameScene);
    addRenderSystem<ColliderRenderSystem>(gameScene);
    addRenderSystem<TilemapRenderSystem>(gameScene);

    setScene(gameScene);
  }
};
