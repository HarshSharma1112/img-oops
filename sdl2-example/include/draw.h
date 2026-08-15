#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <cstring>

static const uint8_t FONT[59][5] = {
    {0, 0, 0, 0, 0},
    {4, 4, 4, 0, 4},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {25,26, 4,11,19},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0,14, 0, 0},
    {0, 0, 0, 0, 4},
    {0, 0,31, 0, 0},
    {0, 0, 0, 0, 4},
    {1, 2, 4, 8,16},
    {14,19,21,25,14},
    {4,12, 4, 4,14},
    {14,17, 6, 8,31},
    {31, 2, 6, 1,30},
    {2, 6,10,31, 2},
    {31,16,30, 1,30},
    {6, 8,30,17,14},
    {31, 1, 2, 4, 8},
    {14,17,14,17,14},
    {14,17,15, 1,14},
    {0, 4, 0, 4, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0,31, 0,31},
    {0, 0, 0, 0, 0},
    {14,17, 6, 0, 4},
    {0, 0, 0, 0, 0},
    {14,17,31,17,17},
    {30,17,30,17,30},
    {14,17,16,17,14},
    {30,17,17,17,30},
    {31,16,30,16,31},
    {31,16,30,16,16},
    {14,17,23,17,14},
    {17,17,31,17,17},
    {14, 4, 4, 4,14},
    {1, 1, 1,17,14},
    {17,18,28,18,17},
    {16,16,16,16,31},
    {17,25,21,19,17},
    {17,25,21,19,17},
    {14,17,17,17,14},
    {30,17,30,16,16},
    {14,17,21,18,13},
    {30,17,30,20,17},
    {15,16,14, 1,30},
    {31, 4, 4, 4, 4},
    {17,17,17,17,14},
    {17,17,17,10, 4},
    {17,17,21,27,17},
    {17,10, 4,10,17},
    {17,17,10, 4, 4},
    {31, 2, 4, 8,31},
};

inline void drawChar(SDL_Renderer* r, char ch, int x, int y, int scale, SDL_Color c) {
    int idx = static_cast<int>(ch) - 32;
    if (idx < 0 || idx >= 59) return;
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    for (int row = 0; row < 5; ++row) {
        uint8_t bits = FONT[idx][row];
        for (int col = 0; col < 5; ++col) {
            if (bits & (1 << (4 - col))) {
                SDL_Rect px = { x + col * scale, y + row * scale, scale, scale };
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

inline void drawText(SDL_Renderer* r, const std::string& s, int x, int y, int scale, SDL_Color c) {
    int cx = x;
    for (char ch : s) {
        drawChar(r, ch, cx, y, scale, c);
        cx += 6 * scale;
    }
}

inline int textWidth(const std::string& s, int scale) {
    return static_cast<int>(s.size()) * 6 * scale;
}

inline void fillRect(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

inline void drawRect(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(r, &rect);
}

inline void drawBar(SDL_Renderer* r, int x, int y, int w, int h, float frac, SDL_Color fill, SDL_Color bg) {
    fillRect(r, x, y, w, h, bg);
    int fw = static_cast<int>(w * frac);
    if (fw > 0) fillRect(r, x, y, fw, h, fill);
    drawRect(r, x, y, w, h, {180, 180, 180, 255});
}

inline bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}
