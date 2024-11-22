#pragma once

#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <SDL_events.h>
#include <SDL_timer.h>
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Player.h"

// Configuración inicial de la cámara
class CameraSetupSystem : public SetupSystem {
public:
  void run() override {
    // Tamaño de la ventana del juego
    int width = 800;  // Ancho de la ventana
    int height = 600; // Alto de la ventana

    // Crear la cámara principal
    scene->mainCamera = scene->createEntity("CAMERA", 0, 0);
    scene->mainCamera->addComponent<CameraComponent>(
        1,         // Zoom inicial
        width,     // Ancho de la ventana
        height,    // Alto de la ventana
        2400,      // Ancho total del mundo (nivel)
        1800       // Alto total del mundo (nivel)
    );
    scene->mainCamera->addComponent<PositionComponent>(0, 0); // Posición inicial de la cámara
  }
};

// Sistema de seguimiento de la cámara
class CameraFollowUpdateSystem : public UpdateSystem {
public:
  void run(float dT) override {
    // Obtener la vista de las entidades que incluyen PlayerComponent, PositionComponent y SpriteComponent
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
