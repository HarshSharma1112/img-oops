#pragma once
#include "GameObject.h"
#include "InputHandler.h"
#include <vector>

class Player : public GameObject {
public:
    Player(float startX, float startY);

    void update(float dt) override;
    void render(SDL_Renderer* r) const override;

    float getHealth() const { return health; }
    float getPower() const { return power; }
    bool hasKeycard() const { return keycard; }

    void takeDamage(float dmg);
    void restorePower(float amt);
    void collectKeycard() { keycard = true; }

    bool isAlive() const { return health > 0.0f; }
    bool hasPower() const { return power > 0.0f; }

    float getDamageFlash() const { return damageFlash; }

    void handleInput(const InputHandler& input, float dt, const std::vector<SDL_Rect>& open);

private:
    float health, power, speed;
    bool keycard;

    float damageFlash;
    float invulTimer;

    void resolveWalls(const std::vector<SDL_Rect>& open);
};
