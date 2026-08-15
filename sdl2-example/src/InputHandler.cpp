#include "../include/InputHandler.h"
#include <cstring>

InputHandler::InputHandler() : current(nullptr) {
    std::memset(previous, 0, sizeof(previous));
}

void InputHandler::update() {
    if (current) {
        std::memcpy(previous, current, SDL_NUM_SCANCODES);
    }
    current = SDL_GetKeyboardState(nullptr);
}

bool InputHandler::isHeld(SDL_Scancode key) const {
    return current && current[key];
}
