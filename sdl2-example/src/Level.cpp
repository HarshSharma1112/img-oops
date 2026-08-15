#include "../include/Level.h"
#include "../include/SecurityBot.h"
#include "../include/draw.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <string>

static constexpr int SW = 1200;
static constexpr int SH = 800;

static SDL_Rect R(int x, int y, int w, int h) { return {x, y, w, h}; }

Level::Level(SDL_Renderer* rend)
    : renderer(rend), player(nullptr), door(nullptr), globalTime(0.0f), shakeTimer(0.0f) {
    buildLayout();
}

Level::~Level() {
    delete player;
    delete door;
    for (Enemy* e : enemies) delete e;
    for (Collectible* c : items) delete c;
}

void Level::buildLayout() {
    rooms = {
        R(340, 65, 520, 180),
        R(575, 245, 50, 80),
        R(100, 325, 1000, 110),
        R(120, 435, 60, 110),
        R(1020, 435, 60, 110),
        R(80, 545, 300, 175),
        R(840, 545, 320, 175),
        R(1020, 720, 60, 28),
        R(840, 720, 320, 28),
    };

    escapePod = R(975, 722, 60, 24);
    player = new Player(580.0f, 365.0f);

    items.push_back(new Collectible(560.0f, 120.0f, CollectibleType::Keycard));
    items.push_back(new Collectible(220.0f, 610.0f, CollectibleType::Battery));
    items.push_back(new Collectible(890.0f, 360.0f, CollectibleType::Battery));

    door = new Door(1020.0f, 715.0f, 60, 10);

    enemies.push_back(new SecurityBot(450.0f, 130.0f, 350.0f, 840.0f));
    enemies.push_back(new SecurityBot(700.0f, 365.0f, 110.0f, 1060.0f));
    enemies.push_back(new SecurityBot(870.0f, 590.0f, 850.0f, 1140.0f));
}

std::vector<SDL_Rect> Level::getWalkable() const {
    std::vector<SDL_Rect> open;
    for (int i = 0; i < 7 && i < static_cast<int>(rooms.size()); ++i)
        open.push_back(rooms[i]);
    if (!door->isLocked()) {
        for (int i = 7; i < static_cast<int>(rooms.size()); ++i)
            open.push_back(rooms[i]);
    }
    return open;
}

void Level::update(float dt, const InputHandler& input) {
    globalTime += dt;
    if (shakeTimer > 0.0f) shakeTimer -= dt;

    auto open = getWalkable();
    player->handleInput(input, dt, open);
    player->update(dt);
    door->update(dt);

    for (Enemy* e : enemies) {
        e->setPlayerPos(player->getX() + player->getW() * 0.5f, player->getY() + player->getH() * 0.5f);
        e->update(dt);
    }
    for (Collectible* c : items) c->update(dt);

    updateParticles(dt);
    checkCollisions();
}

void Level::checkCollisions() {
    SDL_Rect pRect = player->getRect();

    for (Enemy* e : enemies) {
        if (!e->isActive()) continue;
        if (checkCollision(pRect, e->getRect())) {
            if (e->isAttacking()) {
                player->takeDamage(e->getAttackDamage() * 0.016f);
                if (player->getDamageFlash() > 0.9f) {
                    shakeTimer = 0.25f;
                    spawnParticles(player->getX() + 12, player->getY() + 14, {255, 60, 0, 255}, 8);
                }
            }
        }
    }

    for (Collectible* c : items) {
        if (!c->isActive()) continue;
        if (checkCollision(pRect, c->getRect())) {
            SDL_Color sparkCol;
            if (c->getType() == CollectibleType::Keycard) {
                player->collectKeycard();
                sparkCol = {0, 220, 255, 255};
            } else {
                player->restorePower(c->getBatteryAmount());
                sparkCol = {50, 255, 50, 255};
            }
            c->pickup();
            spawnParticles(c->getX(), c->getY(), sparkCol, 12);
        }
    }

    if (door->isLocked() && player->hasKeycard()) {
        if (checkCollision(pRect, door->getRect())) {
            door->unlock();
            spawnParticles(door->getX() + 30, door->getY(), {0, 255, 200, 255}, 16);
        }
    }
}

bool Level::isGameOver() const {
    return !player->isAlive() || !player->hasPower();
}

bool Level::isVictory() const {
    if (!player->hasKeycard()) return false;
    if (door->isLocked()) return false;
    return checkCollision(player->getRect(), escapePod);
}

std::string Level::getObjective() const {
    if (!player->hasKeycard()) return "FIND THE SECURITY KEYCARD";
    if (door->isLocked()) return "UNLOCK THE SECURITY DOOR";
    return "REACH THE ESCAPE POD";
}

void Level::spawnParticles(float cx, float cy, SDL_Color col, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.x = cx; p.y = cy;
        float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.0f;
        float spd = 30.0f + (std::rand() % 80);
        p.vx = std::cos(angle) * spd;
        p.vy = std::sin(angle) * spd;
        p.life = p.maxLife = 0.5f + (std::rand() % 40) * 0.01f;
        p.color = col;
        particles.push_back(p);
    }
}

void Level::updateParticles(float dt) {
    for (Particle& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.life -= dt;
    }
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.life <= 0; }),
        particles.end());
}

void Level::render() {
    SDL_RenderSetViewport(renderer, nullptr);
    renderBackground();
    renderRooms();
    renderObjects();
    renderParticles();
    renderHUD();
}

void Level::renderBackground() {
    fillRect(renderer, 0, 52, SW, SH - 104, {18, 22, 30, 255});
    SDL_SetRenderDrawColor(renderer, 28, 34, 44, 255);
    for (int y = 60; y < SH - 60; y += 40) {
        SDL_RenderDrawLine(renderer, 0, y, SW, y);
    }
    for (int x = 0; x < SW; x += 60) {
        SDL_RenderDrawLine(renderer, x, 52, x, SH - 52);
    }
}

struct RoomStyle {
    SDL_Rect rect;
    const char* name;
    SDL_Color floor;
    SDL_Color light;
};

void Level::renderRooms() {
    const RoomStyle styles[] = {
        {rooms[0], "STORAGE", {30, 40, 55, 255}, {0, 150, 255, 60}},
        {rooms[1], "", {28, 35, 48, 255}, {0, 100, 200, 30}},
        {rooms[2], "CORRIDOR", {25, 30, 40, 255}, {255, 60, 0, 20}},
        {rooms[3], "", {25, 30, 42, 255}, {0, 80, 160, 20}},
        {rooms[4], "", {25, 30, 42, 255}, {0, 80, 160, 20}},
        {rooms[5], "MEDICAL", {28, 42, 50, 255}, {0, 200, 200, 50}},
        {rooms[6], "SECURITY", {42, 28, 28, 255}, {255, 40, 40, 60}},
        {rooms[7], "", {22, 40, 28, 255}, {0, 200, 100, 30}},
        {rooms[8], "ESCAPE POD", {22, 45, 30, 255}, {0, 255, 100, 70}},
    };
    int numStyles = static_cast<int>(sizeof(styles) / sizeof(styles[0]));

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (int i = 0; i < numStyles && i < static_cast<int>(rooms.size()); ++i) {
        const RoomStyle& s = styles[i];
        fillRect(renderer, s.rect.x, s.rect.y, s.rect.w, s.rect.h, s.floor);

        SDL_SetRenderDrawColor(renderer, s.floor.r + 10, s.floor.g + 10, s.floor.b + 10, 180);
        for (int gx = s.rect.x + 20; gx < s.rect.x + s.rect.w; gx += 40)
            SDL_RenderDrawLine(renderer, gx, s.rect.y, gx, s.rect.y + s.rect.h);
        for (int gy = s.rect.y + 20; gy < s.rect.y + s.rect.h; gy += 40)
            SDL_RenderDrawLine(renderer, s.rect.x, gy, s.rect.x + s.rect.w, gy);

        SDL_SetRenderDrawColor(renderer, s.light.r, s.light.g, s.light.b, s.light.a);
        SDL_RenderFillRect(renderer, &s.rect);

        drawRect(renderer, s.rect.x + 1, s.rect.y + 1, s.rect.w - 2, s.rect.h - 2, {80, 90, 110, 255});
        drawRect(renderer, s.rect.x, s.rect.y, s.rect.w, s.rect.h, {110, 120, 140, 255});

        if (s.name[0] != '\0') {
            drawText(renderer, s.name, s.rect.x + 6, s.rect.y + 4, 1, {140, 160, 180, 200});
        }
    }

    float p = (std::sin(globalTime * 1.5f) + 1.0f) * 0.5f;
    Uint8 la = static_cast<Uint8>(60 + 40 * p);
    SDL_Rect mCorr = rooms[2];
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, la);
    SDL_RenderDrawLine(renderer, mCorr.x + 5, mCorr.y + 3, mCorr.x + mCorr.w - 5, mCorr.y + 3);
    SDL_RenderDrawLine(renderer, mCorr.x + 5, mCorr.y + mCorr.h - 4, mCorr.x + mCorr.w - 5, mCorr.y + mCorr.h - 4);

    float blink = std::sin(globalTime * 3.0f);
    if (blink > 0.3f) {
        Uint8 ra = static_cast<Uint8>(80 * blink);
        SDL_SetRenderDrawColor(renderer, 255, 30, 0, ra);
        SDL_RenderDrawLine(renderer, mCorr.x + 80, mCorr.y + mCorr.h / 2, mCorr.x + 140, mCorr.y + mCorr.h / 2);
        SDL_RenderDrawLine(renderer, mCorr.x + mCorr.w - 140, mCorr.y + mCorr.h / 2, mCorr.x + mCorr.w - 80, mCorr.y + mCorr.h / 2);
    }

    float eg = (std::sin(globalTime * 2.0f) + 1.0f) * 0.5f;
    Uint8 eGlow = static_cast<Uint8>(50 + 80 * eg);
    SDL_SetRenderDrawColor(renderer, 0, 255, 120, eGlow);
    SDL_RenderFillRect(renderer, &escapePod);

    drawText(renderer, "ESCAPE", escapePod.x + 4, escapePod.y + 4, 1, {0, 255, 100, 255});

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Level::renderObjects() {
    door->render(renderer);
    for (Collectible* c : items) c->render(renderer);
    for (Enemy* e : enemies) e->render(renderer);
    player->render(renderer);
}

void Level::renderParticles() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const Particle& p : particles) {
        float frac = p.life / p.maxLife;
        Uint8 a = static_cast<Uint8>(frac * 220);
        int sz = std::max(1, static_cast<int>(frac * 5));
        fillRect(renderer, static_cast<int>(p.x) - sz / 2, static_cast<int>(p.y) - sz / 2, sz, sz, {p.color.r, p.color.g, p.color.b, a});
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Level::renderHUD() {
    fillRect(renderer, 0, 0, SW, 52, {12, 15, 22, 255});
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, 180);
    SDL_RenderDrawLine(renderer, 0, 52, SW, 52);

    drawText(renderer, "SPACE STATION ESCAPE", 10, 8, 2, {0, 200, 255, 255});

    int barY = 8;
    drawText(renderer, "HEALTH", 430, barY, 1, {255, 120, 120, 255});
    float hFrac = player->getHealth() / 100.0f;
    SDL_Color hFill = (hFrac > 0.5f) ? SDL_Color{60, 220, 60, 255} : (hFrac > 0.25f) ? SDL_Color{220, 180, 0, 255} : SDL_Color{255, 50, 50, 255};
    drawBar(renderer, 500, barY + 1, 140, 12, hFrac, hFill, {30, 30, 40, 255});

    drawText(renderer, "POWER", 660, barY, 1, {120, 180, 255, 255});
    float pFrac = player->getPower() / 100.0f;
    SDL_Color pFill = (pFrac > 0.3f) ? SDL_Color{60, 120, 255, 255} : SDL_Color{255, 80, 20, 255};
    drawBar(renderer, 720, barY + 1, 130, 12, pFrac, pFill, {30, 30, 40, 255});

    SDL_Color kcCol = player->hasKeycard() ? SDL_Color{0, 255, 200, 255} : SDL_Color{100, 100, 120, 255};
    drawText(renderer, player->hasKeycard() ? "KEYCARD: FOUND" : "KEYCARD: MISSING", 880, barY, 1, kcCol);

    auto pct = [](float f) -> std::string { return std::to_string(static_cast<int>(f * 100)) + "%"; };
    drawText(renderer, pct(hFrac), 648, barY + 1, 1, {200, 200, 200, 200});
    drawText(renderer, pct(pFrac), 858, barY + 1, 1, {200, 200, 200, 200});

    fillRect(renderer, 0, SH - 52, SW, 52, {12, 15, 22, 255});
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, 180);
    SDL_RenderDrawLine(renderer, 0, SH - 52, SW, SH - 52);

    std::string obj = "OBJECTIVE: " + getObjective();
    int tw = textWidth(obj, 2);
    drawText(renderer, obj, SW / 2 - tw / 2, SH - 38, 2, {0, 220, 180, 255});

    if (player->getPower() < 20.0f) {
        float w = (std::sin(globalTime * 5.0f) + 1.0f) * 0.5f;
        Uint8 a = static_cast<Uint8>(150 * w);
        drawText(renderer, "! LOW POWER !", SW / 2 - 65, SH / 2 - 60, 2, {255, 140, 0, a});
    }

    if (player->getHealth() < 25.0f) {
        float v = (std::sin(globalTime * 3.0f) + 1.0f) * 0.5f;
        Uint8 a = static_cast<Uint8>(30 + 40 * v);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, a);
        SDL_Rect vign = {0, 52, SW, SH - 104};
        SDL_RenderDrawRect(renderer, &vign);
        for (int i = 1; i <= 3; ++i) {
            vign = {i, 52 + i, SW - 2 * i, SH - 104 - 2 * i};
            SDL_RenderDrawRect(renderer, &vign);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}
