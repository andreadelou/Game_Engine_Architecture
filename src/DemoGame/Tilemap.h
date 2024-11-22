#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Sprites.h"
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"

// Enumeración de tipos de tiles
enum class TileType {
  NONE,
  WALL,
  GROUND,
  TRIGGER,
};

// Estructura para representar un tile
struct Tile {
  int index;          // Índice del tile en el mapa
  int tilemapIndex;   // Índice del tile en el spritesheet
  TileType type;      // Tipo del tile
};

// Componente para manejar tilemaps
struct TilemapComponent {
  std::string filename;        // Archivo de textura
  std::vector<Tile> tiles;     // Vector de tiles
  int tileSize;                // Tamaño de cada tile
  int scale;                   // Escala de los tiles
  int width;                   // Ancho del mapa en tiles
  int height;                  // Alto del mapa en tiles
};

// Sistema para configurar un tilemap estático
class TilemapSetupSystem : public SetupSystem {
public:
  void run() override {
    std::vector<int> initialMap = {
      0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
      0, 0, 0, 0, 1, 1, 0, 0, 1, 0,
      0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    std::string filename = "src/images/lava.png";
    int tileSize = 8;
    int scale = 8;
    int width = 10;
    int height = 10;

    std::vector<Tile> tiles;
    for (int i = 0; i < initialMap.size(); ++i) {
      TileType type = TileType::NONE;
      if (initialMap[i] == 1) {
        type = TileType::GROUND;
      }
      tiles.push_back(Tile{i, 0, type});
    }

    Entity* tilemapEntity = scene->createEntity("TILEMAP");
    tilemapEntity->addComponent<TilemapComponent>(
      filename,
      tiles,
      tileSize,
      scale,
      width,
      height
    );
    tilemapEntity->addComponent<TextureComponent>(filename);
  }
};

// Sistema de renderizado de tilemaps
class TilemapRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) override {
    auto view = scene->r.view<TilemapComponent, TextureComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto& cameraComponent = scene->mainCamera->get<CameraComponent>();

    for (auto e : view) {
      auto tmap = view.get<TilemapComponent>(e);
      auto tex = view.get<TextureComponent>(e);
      Texture* texture = TextureManager::GetTexture(tex.filename);

      int tileSize = tmap.tileSize * tmap.scale;
      int tilemapWidth = tmap.width;
      int tilemapHeight = tmap.height;

      for (int y = 0; y < tilemapHeight; ++y) {
        for (int x = 0; x < tilemapWidth; ++x) {
          const Tile& tile = tmap.tiles[y * tilemapWidth + x];
          if (tile.type != TileType::NONE) {
            int tileIndexX = tile.tilemapIndex % 8;
            int tileIndexY = tile.tilemapIndex / 8;

            SDL_Rect clip = {
              tileIndexX * tmap.tileSize,
              tileIndexY * tmap.tileSize,
              tmap.tileSize,
              tmap.tileSize,
            };

            texture->render(
              scene->renderer,
              x * tileSize - cameraPosition.x,
              y * tileSize - cameraPosition.y,
              tileSize * cameraComponent.zoom,
              tileSize * cameraComponent.zoom,
              &clip
            );
          }
        }
      }
    }
  }
};
