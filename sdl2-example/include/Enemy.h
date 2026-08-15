#pragma once
#include "GameObject.h"

enum class EnemyState { PATROL, CHASE, ATTACK };

class Enemy : public GameObject {
public:
    Enemy(float x, float y, int w, int h);
    virtual ~Enemy() = default;

    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* r) const = 0;
    virtual void setPlayerPos(float px, float py) = 0;
    virtual float getAttackDamage() const = 0;

    bool isAttacking() const { return state == EnemyState::ATTACK; }

protected:
    EnemyState state;
};
