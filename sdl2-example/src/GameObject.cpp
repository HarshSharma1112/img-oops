#include "../include/GameObject.h"

GameObject::GameObject(float x, float y, int w, int h)
    : x(x), y(y), w(w), h(h), active(true) {}

SDL_Rect GameObject::getRect() const {
    return { static_cast<int>(x), static_cast<int>(y), w, h };
}
