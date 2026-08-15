#pragma once
#include "GameObject.h"

class Door : public GameObject {
public:
    Door(float x, float y, int w, int h);

    void update(float dt) override;
    void render(SDL_Renderer* r) const override;

    bool isLocked() const { return locked; }
    void unlock();

private:
    float glowPhase;
    bool locked;
    float openFraction;
};
