#pragma once
#include <string>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Sprites.h"

struct BackgroundComponent {
  std::string filename;
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");
    const std::string& bgfile = "src/images/Ocean.png"; // Asegúrate de que esta ruta sea válida
    background->addComponent<PositionComponent>(0, 0);
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<SpriteComponent>(
        bgfile,
        400,  // Ancho del sprite
        600,  // Alto del sprite
        1,    // Número de filas en el sprite sheet
        3,    // Número de columnas en el sprite sheet
        500,  // Tiempo de animación en ms
        0,    // Offset x inicial
        0,    // Offset y inicial
        0,    // Rotación
        false // Si es animado
    );
    background->addComponent<BackgroundComponent>(bgfile);
  }
};
