#include "../include/Enemy.h"

Enemy::Enemy(float x, float y, int w, int h)
    : GameObject(x, y, w, h), state(EnemyState::PATROL) {}
