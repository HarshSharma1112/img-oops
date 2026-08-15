#pragma once
#include <SDL2/SDL.h>

class InputHandler {
public:
    InputHandler();

    void update();
    bool isHeld(SDL_Scancode key) const;

private:
    const Uint8* current;
    Uint8 previous[SDL_NUM_SCANCODES];
};
