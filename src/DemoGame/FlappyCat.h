#pragma once
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Sprites.h"
#include "Camera.h"
#include "Tilemap.h"
#include <SDL2/SDL.h>
#include <box2d/box2d.h>

// Constantes físicas
const float PIXELS_TO_METERS = 0.01f;
const float GRAVITY = 9.8f;
const float JUMP_FORCE = 3.0f;
const float FORWARD_FORCE = 2.0f;


// Componente para el mundo físico
struct PhysicsWorldComponent {
    b2World* world;
};

// Componente para cuerpos rígidos
struct RigidBodyComponent {
    b2Body* body;
};

// Sistema de configuración del mundo físico
class PhysicsSetupSystem : public SetupSystem {
public:
    void run() override {
        b2Vec2 gravity(0.0f, GRAVITY);
        b2World* world = new b2World(gravity);

        Entity* worldEntity = scene->createEntity("Physics World");
        worldEntity->addComponent<PhysicsWorldComponent>(world);
    }
};

// Sistema de simulación física
class PhysicsSimulationSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto worldView = scene->r.view<PhysicsWorldComponent>();
        b2World* world = worldView.get<PhysicsWorldComponent>(worldView.front()).world;
        world->Step(dT, 8, 3);
    }
};

// Sistema de configuración del jugador
class PlayerSetupSystem : public SetupSystem {
public:
    void run() override {
        float px = 100;
        float py = 300;
        float size = 8;
        float scale = 10;

        Entity* player = scene->createEntity("PLAYER", px, py);
        player->addComponent<PlayerComponent>();
        player->addComponent<TextureComponent>("src/images/player.png");
        player->addComponent<SpriteComponent>("src/images/player.png", size, size, scale, 4, 1000);

        // Configuración de físicas
        auto worldView = scene->r.view<PhysicsWorldComponent>();
        b2World* world = worldView.get<PhysicsWorldComponent>(worldView.front()).world;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(px * PIXELS_TO_METERS, py * PIXELS_TO_METERS);

        b2Body* body = world->CreateBody(&bodyDef);

        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(
            (size * scale / 2.0f) * PIXELS_TO_METERS,
            (size * scale / 2.0f) * PIXELS_TO_METERS
        );

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;

        body->CreateFixture(&fixtureDef);

        player->addComponent<RigidBodyComponent>(body);
    }
};

// Sistema de actualización de posiciones físicas
class PositionUpdateSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<PositionComponent, RigidBodyComponent>();

        for (auto entity : view) {
            auto& position = view.get<PositionComponent>(entity);
            auto& rigidBody = view.get<RigidBodyComponent>(entity);

            b2Vec2 physical_position = rigidBody.body->GetPosition();
            position.x = physical_position.x / PIXELS_TO_METERS;
            position.y = physical_position.y / PIXELS_TO_METERS;
        }
    }
};

// Sistema de entrada para el jugador
class PlayerInputSystem : public EventSystem {
public:
    void run(SDL_Event event) override {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            auto view = scene->r.view<PlayerComponent, RigidBodyComponent>();

            for (auto entity : view) {
                auto& rigidBody = view.get<RigidBodyComponent>(entity);

                // Resetea la velocidad vertical antes de aplicar el impulso
                b2Vec2 velocity = rigidBody.body->GetLinearVelocity();
                velocity.y = 0;
                rigidBody.body->SetLinearVelocity(velocity);

                // Aplica el impulso hacia arriba
                rigidBody.body->ApplyLinearImpulseToCenter(b2Vec2(0, -JUMP_FORCE), true);
            }
        }
    }
};

// Sistema de movimiento hacia adelante para el jugador
class PlayerForwardSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<PlayerComponent, RigidBodyComponent>();

        for (auto entity : view) {
            auto& rigidBody = view.get<RigidBodyComponent>(entity);
            b2Vec2 velocity = rigidBody.body->GetLinearVelocity();
            velocity.x = FORWARD_FORCE;
            rigidBody.body->SetLinearVelocity(velocity);
        }
    }
};

// Sistema de animación de salto
class PlayerJumpAnimationUpdateSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<PlayerComponent, RigidBodyComponent, SpriteComponent>();

        for (auto entity : view) {
            auto& rigidBody = view.get<RigidBodyComponent>(entity);
            auto& spr = view.get<SpriteComponent>(entity);

            b2Vec2 velocity = rigidBody.body->GetLinearVelocity();
            if (velocity.y < 0) {
                spr.yIndex = 1; // Animación de salto
            } else {
                spr.yIndex = 0; // Animación de caída o reposo
            }
        }
    }
};

// Clase principal del juego
class MyGame : public Game {
public:
    Scene* gameScene;
    entt::registry r;

    MyGame()
        : Game("My Game", 800, 600) {}

    void setup() override {
        gameScene = new Scene("gameplay", r, renderer);

        // Sistemas de configuración
        addSetupSystem<PhysicsSetupSystem>(gameScene);
        addSetupSystem<PlayerSetupSystem>(gameScene);

        // Sistemas de actualización
        addUpdateSystem<PhysicsSimulationSystem>(gameScene);
        addUpdateSystem<PositionUpdateSystem>(gameScene);
        addUpdateSystem<PlayerForwardSystem>(gameScene);
        addUpdateSystem<PlayerJumpAnimationUpdateSystem>(gameScene);

        // Sistemas de entrada
        addEventSystem<PlayerInputSystem>(gameScene);

        // Sistemas de renderizado
        addRenderSystem<SpriteRenderSystem>(gameScene);

        setScene(gameScene);
    }
};
