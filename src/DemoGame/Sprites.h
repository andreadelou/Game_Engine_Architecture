#pragma once
#include <string>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Graphics/TextureManager.h"

// Componente para manejar texturas
struct TextureComponent {
  std::string filename;
};

// Componente para manejar sprites
struct SpriteComponent {
  std::string filename;
  int width;
  int height;
  int scale = 1;
  int animationFrames = 0;
  int animationDuration = 0;
  Uint32 lastUpdate = 0;
  int xIndex = 0;
  int yIndex = 0;
  bool movesWithCamera = true; // Indicador de movimiento relativo a la cámara
};

// Sistema para cargar texturas
class TextureSetupSystem : public SetupSystem {
  void run() {
    auto view = scene->r.view<TextureComponent>();
    for (auto e : view) {
      auto tex = view.get<TextureComponent>(e);
      TextureManager::LoadTexture(tex.filename, scene->renderer);
    }
  }
};

// Sistema para manejar el movimiento de los sprites
class SpriteMovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<SpriteComponent, VelocityComponent>();

    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      // Actualizar el índice de animación según la dirección del movimiento
      if (vel.x == 0) {
        if (spr.yIndex == 1) {
          spr.yIndex = 0;
        } else if (spr.yIndex == 3) {
          spr.yIndex = 2;
        }
      }
      if (vel.x < 0 || vel.y < 0) {
        spr.yIndex = 3; // Movimiento hacia la izquierda o arriba
      }
      if (vel.x > 0 || vel.y > 0) {
        spr.yIndex = 1; // Movimiento hacia la derecha o abajo
      }
    }
  }
};

// Sistema para manejar animaciones de los sprites
class SpriteAnimationSystem : public UpdateSystem {
  void run(float dT) override {
    auto view = scene->r.view<SpriteComponent>();
    Uint32 now = SDL_GetTicks();

    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);

      if (spr.animationFrames > 0) {
        if (spr.lastUpdate == 0) {
          spr.lastUpdate = now;
          continue;
        }
        float timeSinceLastUpdate = now - spr.lastUpdate;

        // Calcular cuántos cuadros deben actualizarse
        int framesToUpdate = timeSinceLastUpdate / spr.animationDuration * spr.animationFrames;

        if (framesToUpdate > 0) {
          spr.xIndex += framesToUpdate;
          spr.xIndex %= spr.animationFrames; // Ciclar entre los cuadros de animación
          spr.lastUpdate = now;
        }
      }
    }
  }
};

// Sistema para renderizar sprites
class SpriteRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<PositionComponent, SpriteComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto& cameraComponent = scene->mainCamera->get<CameraComponent>();

    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);

      Texture* texture = TextureManager::GetTexture(spr.filename);
      SDL_Rect clip = {
        spr.xIndex * spr.width,
        spr.yIndex * spr.height,
        spr.width,
        spr.height,
      };

      if (spr.movesWithCamera) {
        // Renderizar relativo a la posición de la cámara
        texture->render(
          scene->renderer,
          pos.x - cameraPosition.x,
          pos.y - cameraPosition.y,
          spr.width * spr.scale * cameraComponent.zoom,
          spr.height * spr.scale * cameraComponent.zoom,
          &clip
        );
      } else {
        // Renderizar sin considerar la posición de la cámara
        texture->render(
          scene->renderer,
          pos.x,
          pos.y,
          spr.width * spr.scale,
          spr.height * spr.scale,
          &clip
        );
      }
    }
  }
};
