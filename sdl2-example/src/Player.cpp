#include "../include/Player.h"
#include "../include/draw.h"
#include <cmath>
#include <algorithm>

static constexpr int SW = 1200;
static constexpr int SH = 800;

Player::Player(float startX, float startY)
    : GameObject(startX, startY, 24, 28), health(100.0f), power(100.0f), speed(160.0f),
      keycard(false), damageFlash(0.0f), invulTimer(0.0f) {}

void Player::handleInput(const InputHandler& input, float dt, const std::vector<SDL_Rect>& open) {
    float dx = 0, dy = 0;
    if (input.isHeld(SDL_SCANCODE_W) || input.isHeld(SDL_SCANCODE_UP)) dy -= 1;
    if (input.isHeld(SDL_SCANCODE_S) || input.isHeld(SDL_SCANCODE_DOWN)) dy += 1;
    if (input.isHeld(SDL_SCANCODE_A) || input.isHeld(SDL_SCANCODE_LEFT)) dx -= 1;
    if (input.isHeld(SDL_SCANCODE_D) || input.isHeld(SDL_SCANCODE_RIGHT)) dx += 1;

    if (dx != 0 && dy != 0) { dx *= 0.707f; dy *= 0.707f; }

    x += dx * speed * dt;
    y += dy * speed * dt;

    resolveWalls(open);

    x = std::max(0.0f, std::min(x, static_cast<float>(SW - w)));
    y = std::max(52.0f, std::min(y, static_cast<float>(SH - h - 52)));
}

void Player::update(float dt) {
    power -= 10.0f * dt;
    if (power < 0.0f) power = 0.0f;

    if (damageFlash > 0.0f) damageFlash -= dt * 3.0f;
    if (invulTimer > 0.0f) invulTimer -= dt;
}

void Player::takeDamage(float dmg) {
    if (invulTimer > 0.0f) return;
    health -= dmg * 10;
    if (health < 0.0f) health = 0.0f;
    damageFlash = 1.0f;
    invulTimer = 0.6f;
}

void Player::restorePower(float amt) {
    power = std::min(100.0f, power + amt);
}

void Player::resolveWalls(const std::vector<SDL_Rect>& open) {
    SDL_Rect pRect = getRect();
    for (const SDL_Rect& r : open) {
        if (checkCollision(pRect, r)) return;
    }
    float bestDist = 1e9f;
    float bestX = x, bestY = y;
    for (const SDL_Rect& r : open) {
        float cx = std::max(static_cast<float>(r.x), std::min(x, static_cast<float>(r.x + r.w - w)));
        float cy = std::max(static_cast<float>(r.y), std::min(y, static_cast<float>(r.y + r.h - h)));
        float d = (cx - x) * (cx - x) + (cy - y) * (cy - y);
        if (d < bestDist) { bestDist = d; bestX = cx; bestY = cy; }
    }
    x = bestX;
    y = bestY;
}

void Player::render(SDL_Renderer* r) const {
    int px = static_cast<int>(x);
    int py = static_cast<int>(y);
    float flash = std::max(0.0f, damageFlash);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 200, 255, 40);
    SDL_Rect glow = {px - 6, py - 6, w + 12, h + 12};
    SDL_RenderFillRect(r, &glow);

    Uint8 bodyR = static_cast<Uint8>(200 + 55 * flash);
    Uint8 bodyG = static_cast<Uint8>(200 * (1.0f - flash));
    Uint8 bodyB = static_cast<Uint8>(200 * (1.0f - flash));
    fillRect(r, px + 4, py + 10, w - 8, h - 14, {bodyR, bodyG, bodyB, 255});
    fillRect(r, px + 3, py, w - 6, 14, {180, 180, 200, 255});
    fillRect(r, px + 6, py + 2, w - 12, 9, {0, 220, 255, 255});
    fillRect(r, px + w - 9, py + 12, 5, 10, {100, 100, 120, 255});
    fillRect(r, px + 3, py + h - 10, 8, 10, {160, 160, 180, 255});
    fillRect(r, px + w - 11, py + h - 10, 8, 10, {160, 160, 180, 255});
    fillRect(r, px + 2, py + h - 5, 9, 5, {80, 80, 90, 255});
    fillRect(r, px + w - 11, py + h - 5, 9, 5, {80, 80, 90, 255});

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}
