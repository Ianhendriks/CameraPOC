#pragma once
#include <SDL2/SDL_rect.h>

class Camera
{
public:
    Camera(float& screenWidth, float& screenHeight);

    void update();
    SDL_Rect getViewRect() const;
    void move(float& x, float& y);
    void follow(float targetX, float targetY);

    float getX() const;
    float getY() const;
private:
    float x;
    float y;

    float viewportWidth;
    float viewportHeight;

    float speed;
};

