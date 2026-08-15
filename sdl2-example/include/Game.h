#pragma once
#include <SDL2/SDL.h>
#include "InputHandler.h"
#include "Level.h"
#include <string>

enum class GameState { Menu, Playing, GameOver, Victory };

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    GameState state;
    float globalTime;
    InputHandler input;
    Level* level;

    void handleEvents(bool& running);
    void update(float dt);
    void render();
    void shutdown();
    void startNewGame();

    void renderMenu();
    void renderGameOver();
    void renderVictory();

    void drawTextCentered(const std::string& s, int y, int scale, SDL_Color c);
    void drawOverlay(Uint8 alpha);
};
