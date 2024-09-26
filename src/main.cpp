#include <SDL_keyboard.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <string>
#include <vector>
#include <iostream>
#include <SDL.h>
#include <entt/entt.hpp>
#include <SDL_image.h>


// Carga la imagen de fondo
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
    return texture;
}

void updateParallax(float dT, SDL_Rect& backgroundRect, float scrollSpeed) {
    backgroundRect.x -= scrollSpeed * dT;
    if (backgroundRect.x <= -SCREEN_WIDTH) {
        backgroundRect.x = 0;
    }
}



int main() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG); // Inicializa SDL_image para soportar PNG

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Carga la textura de fondo
    SDL_Texture* backgroundTexture = loadTexture("src/images/Ocean.png", renderer);

    // Crear entidades y demás inicializaciones...

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handleInput(e, registry, paddleEntity);
        }

        update(dT, registry, ballEntity, paddleEntity);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        // Renderiza el fondo
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

        // Renderiza otras entidades
        render(renderer, registry);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(backgroundTexture); // Libera la textura
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit(); // Cierra SDL_image
    SDL_Quit();

    return 0;
}
