#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Collectible.h"
#include "Door.h"
#include "InputHandler.h"
#include "draw.h"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

struct Particle {
    float x, y, vx, vy;
    float life, maxLife;
    SDL_Color color;
};

class Level {
public:
    explicit Level(SDL_Renderer* renderer);
    ~Level();

    void update(float dt, const InputHandler& input);
    void render();

    bool isGameOver() const;
    bool isVictory() const;
    std::string getObjective() const;

private:
    SDL_Renderer* renderer;

    Player* player;
    std::vector<Enemy*> enemies;
    std::vector<Collectible*> items;
    Door* door;

    std::vector<SDL_Rect> rooms;
    SDL_Rect escapePod;

    float globalTime;
    float shakeTimer;

    std::vector<Particle> particles;

    void buildLayout();
    void checkCollisions();
    void updateParticles(float dt);
    void spawnParticles(float cx, float cy, SDL_Color col, int count);

    void renderBackground();
    void renderRooms();
    void renderObjects();
    void renderParticles();
    void renderHUD();

    std::vector<SDL_Rect> getWalkable() const;
};
