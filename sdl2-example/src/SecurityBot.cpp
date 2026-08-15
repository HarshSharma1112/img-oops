#include "../include/SecurityBot.h"
#include "../include/draw.h"
#include <cmath>
#include <algorithm>

SecurityBot::SecurityBot(float startX, float startY, float patrolMinX, float patrolMaxX)
    : Enemy(startX, startY, 28, 30), patrolMin(patrolMinX), patrolMax(patrolMaxX),
      patrolDir(1.0f), playerX(0.0f), playerY(0.0f), eyeGlow(0.0f), alertTimer(0.0f), animTimer(0.0f) {}

void SecurityBot::setPlayerPos(float px, float py) {
    playerX = px;
    playerY = py;
}

void SecurityBot::update(float dt) {
    animTimer += dt;
    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;
    float dx = playerX - cx;
    float dy = playerY - cy;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < ATTACK_RANGE) {
        state = EnemyState::ATTACK;
    } else if (dist < DETECT_RANGE) {
        state = EnemyState::CHASE;
        alertTimer = 0.3f;
    } else {
        state = EnemyState::PATROL;
    }

    if (alertTimer > 0.0f) alertTimer -= dt;

    float pulseSpeed = (state == EnemyState::PATROL) ? 2.0f : 5.0f;
    eyeGlow = (std::sin(animTimer * pulseSpeed) + 1.0f) * 0.5f;

    switch (state) {
    case EnemyState::PATROL:
        x += patrolDir * PATROL_SPEED * dt;
        if (x > patrolMax - w) patrolDir = -1.0f;
        if (x < patrolMin) patrolDir = 1.0f;
        break;
    case EnemyState::CHASE:
        if (dist > 0.1f) {
            x += (dx / dist) * CHASE_SPEED * dt;
            y += (dy / dist) * CHASE_SPEED * dt;
        }
        break;
    case EnemyState::ATTACK:
        break;
    }
}

void SecurityBot::render(SDL_Renderer* r) const {
    int bx = static_cast<int>(x);
    int by = static_cast<int>(y);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    if (state != EnemyState::PATROL) {
        Uint8 ga = static_cast<Uint8>(30 + 50 * eyeGlow);
        SDL_SetRenderDrawColor(r, 255, 50, 0, ga);
        SDL_Rect glow = {bx - 8, by - 8, w + 16, h + 16};
        SDL_RenderFillRect(r, &glow);
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    fillRect(r, bx + 2, by + 8, w - 4, h - 12, {60, 65, 70, 255});
    fillRect(r, bx + 5, by + 12, w - 10, 8, {80, 40, 20, 255});
    fillRect(r, bx + 3, by, w - 6, 12, {50, 55, 60, 255});

    Uint8 eyeR = 255;
    Uint8 eyeG = (state == EnemyState::ATTACK) ? 0 : (state == EnemyState::CHASE) ? static_cast<Uint8>(80 * eyeGlow) : static_cast<Uint8>(60 * (1.0f - eyeGlow));
    Uint8 eyeA = static_cast<Uint8>(180 + 75 * eyeGlow);
    fillRect(r, bx + 5, by + 3, 5, 5, {eyeR, eyeG, 0, eyeA});
    fillRect(r, bx + w - 10, by + 3, 5, 5, {eyeR, eyeG, 0, eyeA});

    SDL_SetRenderDrawColor(r, 140, 140, 160, 255);
    SDL_RenderDrawLine(r, bx + w / 2, by, bx + w / 2, by - 8);
    fillRect(r, bx + w / 2 - 2, by - 10, 4, 4, {255, 80, 0, 255});

    float legAnim = std::sin(animTimer * 6.0f) * 3.0f;
    int legOff = (state == EnemyState::PATROL) ? static_cast<int>(legAnim) : 0;
    fillRect(r, bx + 4, by + h - 10, 8, 10 + legOff, {50, 55, 60, 255});
    fillRect(r, bx + w - 12, by + h - 10, 8, 10 - legOff, {50, 55, 60, 255});
    fillRect(r, bx + 2, by + h - 3, 10, 5, {40, 40, 50, 255});
    fillRect(r, bx + w - 12, by + h - 3, 10, 5, {40, 40, 50, 255});

    if (alertTimer > 0.0f) {
        drawRect(r, bx - 2, by - 2, w + 4, h + 4, {255, 220, 0, 200});
    }
}
