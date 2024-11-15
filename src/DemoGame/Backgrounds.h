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
    const std::string& bgfile = "src/images/Ocean.png";
    background->addComponent<PositionComponent>(0, 0);
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<SpriteComponent>(bgfile, 400, 600, 1, 3, 500);
    background->addComponent<BackgroundComponent>(bgfile);
  }
};