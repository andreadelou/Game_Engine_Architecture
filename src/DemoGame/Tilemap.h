#pragma once
#include <string>
#include "Sprites.h"
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include <iostream>
#include <unordered_map>

// Enum para definir los tipos de tiles
enum class TileType {
  NONE,
  WALL,
  TRIGGER,
};

// Estructura que representa un Tile
struct Tile {
  int index;         // Índice en el mapa
  int tilemapIndex;  // Índice en el tileset
  TileType type;     // Tipo de tile
};

struct TileComponent {
  Tile tile;
};

// Componente que almacena los datos de un tile
struct TilemapComponent {
  std::string filename;
  std::vector<Tile> tiles;
  int tileSize;
  int scale;
  int width;
  int height;
};

// Sistema de configuración del tilemap
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
    for (int i = 0; i < initialMap.size(); i++) {
      TileType type = TileType::NONE;
      if (initialMap[i] == 1) {
        type = TileType::WALL;
      }
      tiles.push_back(Tile{i, 0, type});
    }

    Entity* tilemap = scene->createEntity("TILEMAP");
    tilemap->addComponent<TilemapComponent>(
      filename,
      tiles,
      tileSize,
      scale,
      width,
      height
    );
    tilemap->addComponent<TextureComponent>(filename);
  }
};

// Sistema para realizar autotiling avanzado
class AdvancedAutoTilingSetupSystem : public SetupSystem {
private:
  std::unordered_map<int, int> maskToTileIndex = {
    {2, 1}, {8, 2}, {10, 3}, {11, 4}, {16, 5}, {18, 6}, {22, 7}, {24, 8},
    {26, 9}, {27, 10}, {30, 11}, {31, 12}, {64, 13}, {66, 14}, {72, 15},
    {74, 16}, {75, 17}, {80, 18}, {82, 19}, {86, 20}, {88, 21}, {90, 22},
    {91, 23}, {94, 24}, {95, 25}, {104, 26}, {106, 27}, {107, 28}, {120, 29},
    {122, 30}, {123, 31}, {126, 32}, {127, 33}, {208, 34}, {210, 35},
    {214, 36}, {216, 37}, {218, 38}, {219, 39}, {222, 40}, {223, 41},
    {248, 42}, {250, 43}, {251, 44}, {254, 45}, {255, 46}, {0, 47}
  };

  bool isTile(const std::vector<Tile>& map, int x, int y, int w, int h) {
    return (x >= 0 && x < w && y >= 0 && y < h && map[y * w + x].type == TileType::WALL);
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
      auto& tilemap = view.get<TilemapComponent>(entity);
      const int width = tilemap.width;
      const int height = tilemap.height;

      std::vector<Tile> newMap = tilemap.tiles;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          if (tilemap.tiles[y * width + x].type == TileType::WALL) {
            int mask = 0;

            if (isTile(tilemap.tiles, x, y - 1, width, height)) mask |= 2;
            if (isTile(tilemap.tiles, x - 1, y, width, height)) mask |= 8;
            if (isTile(tilemap.tiles, x + 1, y, width, height)) mask |= 16;
            if (isTile(tilemap.tiles, x, y + 1, width, height)) mask |= 64;

            auto it = maskToTileIndex.find(mask);
            if (it != maskToTileIndex.end()) {
              newMap[y * width + x].tilemapIndex = it->second;
            } else {
              newMap[y * width + x].tilemapIndex = 47; // Default tile
            }
          } else {
            newMap[y * width + x].tilemapIndex = -1;
          }
        }
      }

      tilemap.tiles = newMap;
    }
  }
};

// Sistema para renderizar el tilemap
class TilemapRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<TilemapComponent, TextureComponent>();
    for (auto e : view) {
      auto& tmap = view.get<TilemapComponent>(e);
      auto& tex = view.get<TextureComponent>(e);

      Texture* texture = TextureManager::GetTexture(tex.filename);

      int tileSize = tmap.tileSize * tmap.scale;
      int tilemapWidth = tmap.width;
      int tilemapHeight = tmap.height;

      for (int y = 0; y < tilemapHeight; y++) {
        for (int x = 0; x < tilemapWidth; x++) {
          int tileIndex = tmap.tiles[y * tilemapWidth + x].tilemapIndex;

          if (tileIndex >= 0) {
            int tileIndexX = tileIndex % 8;
            int tileIndexY = tileIndex / 8;

            SDL_Rect clip = {
              tileIndexX * tmap.tileSize,
              tileIndexY * tmap.tileSize,
              tmap.tileSize,
              tmap.tileSize,
            };

            texture->render(
              scene->renderer,
              x * tileSize,
              y * tileSize,
              tileSize,
              tileSize,
              &clip
            );
          }
        }
      }
    }
  }
};
