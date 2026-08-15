#pragma once
#include <SDL2/SDL.h>

class GameObject {
protected:
    float x, y;
    int   w, h;
    bool  active;

public:
    GameObject(float x, float y, int w, int h);
    virtual ~GameObject() = default;

    float getX() const { return x; }
    float getY() const { return y; }
    int getW() const { return w; }
    int getH() const { return h; }
    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }

    SDL_Rect getRect() const;

    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* r) const = 0;
};
