#include "../include/Door.h"
#include "../include/draw.h"
#include <cmath>
#include <algorithm>

Door::Door(float x, float y, int w, int h)
    : GameObject(x, y, w, h), glowPhase(0.0f), locked(true), openFraction(0.0f) {}

void Door::update(float dt) {
    glowPhase += dt;
    if (!locked && openFraction < 1.0f) {
        openFraction = std::min(1.0f, openFraction + dt * 0.8f);
    }
}

void Door::unlock() {
    locked = false;
}

void Door::render(SDL_Renderer* r) const {
    int dx = static_cast<int>(x);
    int dy = static_cast<int>(y);
    int slideOffset = static_cast<int>(openFraction * h);

    if (slideOffset < h) {
        fillRect(r, dx, dy + slideOffset, w, h - slideOffset, {55, 60, 70, 255});
        SDL_SetRenderDrawColor(r, 80, 85, 95, 255);
        SDL_RenderDrawLine(r, dx + w / 2, dy + slideOffset, dx + w / 2, dy + h);

        float pulse = (std::sin(glowPhase * 4.0f) + 1.0f) * 0.5f;
        SDL_Color lightCol = locked
            ? SDL_Color{255, static_cast<Uint8>(40 * pulse), 0, 255}
            : SDL_Color{0, static_cast<Uint8>(180 + 75 * pulse), 255, 255};

        fillRect(r, dx + w / 2 - 5, dy + slideOffset + 6, 10, 10, lightCol);

        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        Uint8 ga = locked ? static_cast<Uint8>(60 + 60 * pulse) : static_cast<Uint8>(40 + 40 * pulse);
        SDL_SetRenderDrawColor(r, lightCol.r, lightCol.g, lightCol.b, ga);
        SDL_Rect gr = {dx + w / 2 - 10, dy + slideOffset + 2, 20, 18};
        SDL_RenderFillRect(r, &gr);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

        drawRect(r, dx, dy + slideOffset, w, h - slideOffset, {120, 130, 150, 255});
    }
}
