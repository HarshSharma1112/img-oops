#pragma once
#include "GameObject.h"

enum class CollectibleType { Keycard, Battery };

class Collectible : public GameObject {
public:
    Collectible(float x, float y, CollectibleType type);

    void update(float dt) override;
    void render(SDL_Renderer* r) const override;

    CollectibleType getType() const { return type; }
    void pickup() { active = false; }
    float getBatteryAmount() const { return 35.0f; }

private:
    CollectibleType type;
    float bobTimer;
    float glowTimer;

    void drawKeycard(SDL_Renderer* r) const;
    void drawBattery(SDL_Renderer* r) const;
};
