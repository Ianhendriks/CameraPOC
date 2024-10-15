#include "Camera.h"

Camera::Camera(float& screenWidth, float& screenHeight) :
	x {0.0f},
	y {0.0f},
	viewportWidth {screenWidth},
	viewportHeight {screenHeight} {}

void Camera::move(float& targetX, float& targetY)
{
    // Calculate the desired camera position to center on the target
    x = targetX - (viewportWidth / 2.00f);
    y = targetY - (viewportHeight / 2.00f);
}

SDL_Rect Camera::getViewRect() const
{
    SDL_Rect viewRect;
    viewRect.x = static_cast<int>(x);
    viewRect.y = static_cast<int>(y);
    viewRect.w = viewportWidth;
    viewRect.h = viewportHeight;
    return viewRect;
}

float Camera::getX() const { return x; }
float Camera::getY() const { return y; }


