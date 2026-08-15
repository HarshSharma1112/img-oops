#include "../include/Game.h"
#include "../include/draw.h"
#include <iostream>
#include <cmath>

static constexpr int SW = 1200;
static constexpr int SH = 800;
static constexpr int FPS = 60;
static constexpr int FRAME_MS = 1000 / FPS;

Game::Game() : window(nullptr), renderer(nullptr), state(GameState::Menu), globalTime(0.0f), level(nullptr) {}

Game::~Game() { shutdown(); }

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "[ERROR] SDL_Init: " << SDL_GetError() << "\n";
        return false;
    }
    window = SDL_CreateWindow("Space Station Escape", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SW, SH, 0);
    if (!window) {
        std::cerr << "[ERROR] SDL_CreateWindow: " << SDL_GetError() << "\n";
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        if (!renderer) {
            std::cerr << "[ERROR] SDL_CreateRenderer: " << SDL_GetError() << "\n";
            return false;
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return true;
}

void Game::run() {
    Uint32 lastTick = SDL_GetTicks();
    bool running = true;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTick) / 1000.0f;
        lastTick = now;
        if (dt > 0.05f) dt = 0.05f;
        globalTime += dt;

        handleEvents(running);
        update(dt);
        render();

        Uint32 elapsed = SDL_GetTicks() - now;
        if (elapsed < static_cast<Uint32>(FRAME_MS))
            SDL_Delay(FRAME_MS - elapsed);
    }
}

void Game::handleEvents(bool& running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) { running = false; return; }

        if (ev.type == SDL_KEYDOWN) {
            SDL_Scancode key = ev.key.keysym.scancode;

            if (key == SDL_SCANCODE_ESCAPE) {
                if (state == GameState::Playing) state = GameState::Menu;
                else running = false;
            }

            if (key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_KP_ENTER) {
                if (state == GameState::Menu || state == GameState::GameOver || state == GameState::Victory)
                    startNewGame();
            }
        }
    }
    input.update();
}

void Game::update(float dt) {
    if (state != GameState::Playing) return;

    level->update(dt, input);

    if (level->isGameOver()) state = GameState::GameOver;
    if (level->isVictory()) state = GameState::Victory;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 8, 10, 16, 255);
    SDL_RenderClear(renderer);

    if (state == GameState::Playing) {
        level->render();
    } else if (state == GameState::Menu) {
        renderMenu();
    } else if (state == GameState::GameOver) {
        if (level) level->render();
        renderGameOver();
    } else if (state == GameState::Victory) {
        if (level) level->render();
        renderVictory();
    }

    SDL_RenderPresent(renderer);
}

void Game::renderMenu() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 80; ++i) {
        float phase = i * 1.37f + globalTime * 0.2f;
        int sx = static_cast<int>((std::sin(phase) * 0.5f + 0.5f) * SW);
        int sy = static_cast<int>((std::cos(phase * 0.71f) * 0.5f + 0.5f) * SH);
        float bright = (std::sin(phase * 3.0f) + 1.0f) * 0.5f;
        Uint8 b = static_cast<Uint8>(120 + 120 * bright);
        SDL_SetRenderDrawColor(renderer, b, b, 255, static_cast<Uint8>(80 + 80 * bright));
        SDL_RenderDrawPoint(renderer, sx, sy);
        SDL_RenderDrawPoint(renderer, sx + 1, sy);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    int pw = 700, ph = 420;
    int px = (SW - pw) / 2, py = (SH - ph) / 2;
    fillRect(renderer, px, py, pw, ph, {10, 18, 28, 230});
    drawRect(renderer, px, py, pw, ph, {0, 160, 255, 200});
    drawRect(renderer, px + 3, py + 3, pw - 6, ph - 6, {0, 80, 140, 100});

    float g = (std::sin(globalTime * 2.0f) + 1.0f) * 0.5f;
    Uint8 gc = static_cast<Uint8>(180 + 75 * g);
    drawTextCentered("SPACE STATION", py + 30, 4, {gc, gc, 255, 255});
    drawTextCentered("ESCAPE", py + 72, 5, {0, gc, 255, 255});

    drawTextCentered("THE STATION IS FAILING.", py + 148, 2, {180, 180, 200, 210});
    drawTextCentered("YOU HAVE MINUTES BEFORE", py + 170, 2, {160, 160, 180, 190});
    drawTextCentered("THE POWER RUNS OUT.", py + 192, 2, {160, 160, 180, 190});

    fillRect(renderer, px + 60, py + 224, pw - 120, 90, {5, 12, 20, 200});
    drawRect(renderer, px + 60, py + 224, pw - 120, 90, {0, 80, 120, 180});

    struct { const char* key; const char* desc; } ctrl[] = {
        {"WASD / ARROWS", "Move astronaut"},
        {"KEYCARD", "Unlocks the security door"},
        {"BATTERIES", "Restore power supply"},
    };
    for (int i = 0; i < 3; ++i) {
        int ty = py + 234 + i * 26;
        drawText(renderer, ctrl[i].key, px + 80, ty, 1, {0, 200, 255, 255});
        drawText(renderer, "-", px + 222, ty, 1, {100, 120, 140, 200});
        drawText(renderer, ctrl[i].desc, px + 240, ty, 1, {160, 180, 200, 220});
    }

    if (std::sin(globalTime * 3.0f) > 0.0f) {
        drawTextCentered("PRESS ENTER TO START", py + ph - 50, 2, {0, 255, 200, 255});
    }

    drawTextCentered("ESC - QUIT", py + ph - 26, 1, {100, 110, 130, 180});
}

void Game::renderGameOver() {
    drawOverlay(170);
    drawTextCentered("GAME OVER", SH / 2 - 90, 5, {220, 40, 40, 255});
    drawTextCentered("THE STATION CLAIMED YOU.", SH / 2 - 20, 2, {180, 120, 120, 220});
    drawTextCentered("PRESS ENTER TO RETRY", SH / 2 + 30, 2, {180, 180, 180, 200});
    drawTextCentered("ESC - MENU", SH / 2 + 60, 1, {120, 120, 140, 180});
}

void Game::renderVictory() {
    drawOverlay(160);
    float g = (std::sin(globalTime * 3.0f) + 1.0f) * 0.5f;
    Uint8 gc = static_cast<Uint8>(200 + 55 * g);
    drawTextCentered("MISSION COMPLETE", SH / 2 - 90, 4, {gc, 255, gc, 255});
    drawTextCentered("YOU ESCAPED THE STATION!", SH / 2 - 30, 2, {160, 220, 160, 220});
    drawTextCentered("PRESS ENTER TO PLAY AGAIN", SH / 2 + 20, 2, {180, 180, 180, 200});
}

void Game::startNewGame() {
    delete level;
    level = new Level(renderer);
    state = GameState::Playing;
}

void Game::shutdown() {
    delete level;
    level = nullptr;
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
}

void Game::drawTextCentered(const std::string& s, int y, int scale, SDL_Color c) {
    int tw = textWidth(s, scale);
    drawText(renderer, s, SW / 2 - tw / 2, y, scale, c);
}

void Game::drawOverlay(Uint8 alpha) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_Rect full = {0, 0, SW, SH};
    SDL_RenderFillRect(renderer, &full);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
