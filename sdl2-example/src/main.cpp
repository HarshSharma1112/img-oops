#include "../include/Game.h"
#include <iostream>

int main(int, char*[]) {
    Game game;
    if (!game.init()) {
        std::cerr << "[FATAL] Initialisation failed.\n";
        return 1;
    }
    game.run();
    return 0;
}
