#include "../include/Collectible.h"
#include "../include/draw.h"
#include <cmath>

Collectible::Collectible(float x, float y, CollectibleType type)
    : GameObject(x, y, 22, 22), type(type), bobTimer(0.0f), glowTimer(0.0f) {}

void Collectible::update(float dt) {
    if (!active) return;
    bobTimer += dt;
    glowTimer += dt;
}

void Collectible::render(SDL_Renderer* r) const {
    if (!active) return;
    if (type == CollectibleType::Keycard) drawKeycard(r);
    else drawBattery(r);
}

void Collectible::drawKeycard(SDL_Renderer* r) const {
    float bob = std::sin(bobTimer * 3.0f) * 4.0f;
    float glow = (std::sin(glowTimer * 4.0f) + 1.0f) * 0.5f;
    int bx = static_cast<int>(x);
    int by = static_cast<int>(y + bob);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    Uint8 ga = static_cast<Uint8>(40 + 60 * glow);
    SDL_SetRenderDrawColor(r, 0, 220, 255, ga);
    SDL_Rect gr = {bx - 8, by - 8, w + 16, h + 16};
    SDL_RenderFillRect(r, &gr);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    fillRect(r, bx, by, w, h, {0, 160, 200, 255});
    fillRect(r, bx + 2, by + 2, w - 4, h - 4, {0, 200, 255, 255});
    fillRect(r, bx + 2, by + 5, w - 4, 4, {0, 100, 140, 255});
    fillRect(r, bx + 5, by + 12, 7, 5, {220, 180, 0, 255});
    drawRect(r, bx, by, w, h, {0, 255, 255, 255});
}

void Collectible::drawBattery(SDL_Renderer* r) const {
    float bob = std::sin(bobTimer * 2.5f) * 3.0f;
    float glow = (std::sin(glowTimer * 3.0f) + 1.0f) * 0.5f;
    int bx = static_cast<int>(x);
    int by = static_cast<int>(y + bob);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    Uint8 ga = static_cast<Uint8>(30 + 50 * glow);
    SDL_SetRenderDrawColor(r, 50, 255, 50, ga);
    SDL_Rect gr = {bx - 6, by - 6, w + 12, h + 12};
    SDL_RenderFillRect(r, &gr);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    fillRect(r, bx, by + 3, w, h - 3, {30, 100, 30, 255});
    fillRect(r, bx + 2, by + 5, w - 4, h - 7, {50, 180, 50, 255});
    fillRect(r, bx + 7, by, w - 14, 4, {80, 200, 80, 255});
    for (int i = 0; i < 3; ++i) {
        fillRect(r, bx + 4 + i * 5, by + 7, 3, h - 12, {150, 255, 100, 255});
    }
    drawRect(r, bx, by + 3, w, h - 3, {80, 255, 80, 255});
}
