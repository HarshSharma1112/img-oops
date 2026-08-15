#pragma once
#include "Enemy.h"

class SecurityBot : public Enemy {
public:
    SecurityBot(float startX, float startY, float patrolMinX, float patrolMaxX);

    void update(float dt) override;
    void render(SDL_Renderer* r) const override;
    void setPlayerPos(float px, float py) override;
    float getAttackDamage() const override { return 15.0f; }

private:
    float patrolMin, patrolMax;
    float patrolDir;
    float playerX, playerY;

    float eyeGlow;
    float alertTimer;
    float animTimer;

    static constexpr float PATROL_SPEED = 80.0f;
    static constexpr float CHASE_SPEED = 140.0f;
    static constexpr float DETECT_RANGE = 200.0f;
    static constexpr float ATTACK_RANGE = 35.0f;

    void drawBot(SDL_Renderer* r) const;
};
