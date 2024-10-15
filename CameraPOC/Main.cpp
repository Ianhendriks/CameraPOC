#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Camera.h"

struct Sprite {
    SDL_Texture* texture;
    float x, y;
    int w, h;
};

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Function to render a texture relative to the camera
void render(SDL_Renderer* renderer, const Sprite& sprite, const Camera& camera) {
    SDL_Rect destRect;
    destRect.x = static_cast<int>(sprite.x - camera.getX());
    destRect.y = static_cast<int>(sprite.y - camera.getY());
    destRect.w = sprite.w;
    destRect.h = sprite.h;

    // Culling: Only render if within camera's view
    SDL_Rect view = camera.getViewRect();
    if (destRect.x + destRect.w < 0 || destRect.y + destRect.h < 0 ||
        destRect.x > view.w || destRect.y > view.h) {
        return; // Skip rendering
    }

    SDL_RenderCopy(renderer, sprite.texture, nullptr, &destRect);
}

// Function to render the health bar
void renderHealthBar(SDL_Renderer* renderer, const float& healthPercentage) {
    int barWidth = 200;   
    int barHeight = 20;   
    int padding = 10;      
    int xPosition = 800 - barWidth - padding;
    int yPosition = 600 - barHeight - padding; 

    // Draw the background outline of the health bar
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect outlineRect = { xPosition - 2, yPosition - 2, barWidth + 4, barHeight + 4 };
    SDL_RenderFillRect(renderer, &outlineRect);

    // Draw the background of the health bar
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect backgroundRect = { xPosition, yPosition, barWidth, barHeight };
    SDL_RenderFillRect(renderer, &backgroundRect);

    // Draw the health part of the health bar
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect healthRect = { xPosition, yPosition, static_cast<int>(barWidth * (healthPercentage / 100.0f)), barHeight };
    SDL_RenderFillRect(renderer, &healthRect);
}

int main()
{
    float WORLD_WIDTH = 2000.00f;
    float WORLD_HEIGHT = 2000.00f;
    float SCREEN_WIDTH = 800.00f;
    float SCREEN_HEIGHT = 600.00f;

    SDL_Init(SDL_INIT_VIDEO) != 0;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL2 Camera Movement",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* spriteTexture = loadTexture("Texture1.png", renderer);

    Sprite player;
    player.x = 400.0f;
    player.y = 300.0f;
    player.w = 64;
    player.h = 64;
    player.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, 8, 8);

    std::vector<Sprite> sprites;
    for (int i = 0; i < 20; ++i) {
        Sprite sprite;
        sprite.texture = spriteTexture;
        sprite.x = i * 100.0f;
        sprite.y = i * 50.0f;
        sprite.w = 64;
        sprite.h = 64;
        sprites.push_back(sprite);
    }

    Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
    float speed = 300.00f;
    Uint32 lastTime = SDL_GetTicks();
    float health = 100.00f;

    bool running = true;
    SDL_Event event;

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
            moveX -= speed * deltaTime;
        }
        if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
            moveX += speed * deltaTime;
        }
        if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
            moveY -= speed * deltaTime;
        }
        if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {
            moveY += speed * deltaTime;
        }
        if (state[SDL_SCANCODE_Q]) {
            health -= 2;
            if (health < 0) health = 0;
        }
        if (state[SDL_SCANCODE_E]) { 
            health += 2;
            if (health > 100) health = 100;
        }
        
        float newX = player.x += moveX;
        float newY = player.y += moveY;

        player.x = newX;
        player.y = newY;
        float middleX = player.x + player.w / 2.00f;
        float middleY = player.y + player.h / 2.00f;

        // Clamp player within world boundaries
        if (player.x < 0)
            player.x = 0;
        if (player.y < 0)
            player.y = 0;
        if (player.x + player.w > WORLD_WIDTH)
            player.x = WORLD_WIDTH - player.w;
        if (player.y + player.h > WORLD_HEIGHT)
            player.y = WORLD_HEIGHT - player.h;

        camera.move(middleX, middleY);
        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
        SDL_RenderClear(renderer);

        // Render all sprites relative to camera
        for (const auto& sprite : sprites) {
            render(renderer, sprite, camera);
        }
        render(renderer, player, camera);

        renderHealthBar(renderer, health);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(spriteTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}