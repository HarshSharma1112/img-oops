# Space Station Escape 🚀👨‍🚀

> **A 2D sci-fi survival game built in C++17 with SDL2 for OOP evaluation.**  
> Explore a damaged space station, manage declining power, avoid hostile security robots, locate the keycard, unlock the door, and reach the escape pod.

---

## Table of Contents
1. [Game Concept](#game-concept)
2. [Controls](#controls)
3. [How to Build & Run](#how-to-build--run)
4. [File Structure](#file-structure)
5. [Class Architecture & Relationships](#class-architecture--relationships)
6. [OOP Concepts Demonstrated](#oop-concepts-demonstrated)
7. [Game Loop Explanation](#game-loop-explanation)
8. [Enemy AI & State Machine](#enemy-ai--state-machine)
9. [Collision System](#collision-system)

---

## Game Concept

You play as an astronaut trapped inside a failing space station. 

### Core Gameplay Mechanics:
* **Health**: Damaged when attacked by SecurityBots. Game over if health hits 0%.
* **Power**: Constantly drains over time (2%/sec). Collect **Batteries** to restore power (+35%). Game over if power reaches 0%.
* **Security Keycard**: Hidden in the Storage Room. Required to unlock the Security Door leading to the Escape Pod.
* **Security Door**: Blocks passage to the escape chamber until unlocked with the Keycard.
* **Escape Pod**: Reach the pod after opening the door to win the game!

---

## Controls

| Key / Action | Function |
|---|---|
| **W / A / S / D** or **Arrow Keys** | Move Astronaut |
| **ENTER** | Start Game / Retry on Game Over or Victory |
| **ESC** | Return to Menu / Quit Game |

---

## How to Build & Run

### Prerequisites
* Linux environment with `g++` (supporting C++17)
* `make` build tool
* SDL2 development libraries (bundled locally in `./sdl2_local/`)

### Build Commands
```bash
# Compile the game
make

# Run the game
./game

# Clean build artifacts
make clean
```

---

## File Structure

```text
sdl2-example/
├── include/
│   ├── Collectible.h     # Keycard & Battery pickup class
│   ├── Door.h            # Security door class
│   ├── draw.h            # Header-only SDL draw utils & 5x5 bitmap font
│   ├── Enemy.h           # Abstract base class for enemies
│   ├── Game.h            # High-level SDL lifecycle & state controller
│   ├── GameObject.h      # Abstract base class for all game entities
│   ├── InputHandler.h    # Keyboard snapshot & just-pressed detection
│   ├── Level.h           # Game world, room layout, logic & HUD
│   ├── Player.h          # Player astronaut class
│   └── SecurityBot.h     # Concrete enemy class (patrol/chase/attack)
├── src/
│   ├── Collectible.cpp
│   ├── Door.cpp
│   ├── Enemy.cpp
│   ├── Game.cpp
│   ├── GameObject.cpp
│   ├── InputHandler.cpp
│   ├── Level.cpp
│   ├── main.cpp          # Entry point
│   ├── Player.cpp
│   └── SecurityBot.cpp
├── Makefile              # C++17 / SDL2 build configuration
├── README.md             # Overview & documentation
└── VIVA_GUIDE.md         # Comprehensive Q&A for student evaluation
```

---

## Demo Video

[Screencast from 2026-08-15 10-27-04.webm](https://github.com/user-attachments/assets/c2c7f309-3444-45ac-acfa-3f43f3b85b20)

---

## Class Architecture & Relationships

The project features **9 core classes**:

```text
                     ┌──────────────────┐
                     │    GameObject    │ (Abstract Base)
                     └────────┬─────────┘
                              │
     ┌────────────────────────┼────────────────────────┬──────────────────┐
     │                        │                        │                  │
┌────┴───┐               ┌────┴───┐               ┌────┴─────┐       ┌────┴─┐
│ Player │               │  Enemy │ (Abstract)    │Collectible│       │ Door │
└────────┘               └────┬───┘               └──────────┘       └──────┘
                              │
                     ┌────────┴──────┐
                     │  SecurityBot  │
                     └───────────────┘

┌──────────────┐          owns 1          ┌───────────┐
│     Game     │ ───────────────────────> │   Level   │ (Composes Player, Enemies,
└──────────────┘                          └───────────┘  Items, Door, Rooms)
```

---

## OOP Concepts Demonstrated

1. **Class & Object**: Encapsulation of entity data and behavior into modular classes.
2. **Inheritance**: `Player`, `Enemy`, `Collectible`, and `Door` inherit position/active state from `GameObject`. `SecurityBot` inherits from `Enemy`.
3. **Abstraction**: High-level callers interact through simple interfaces (e.g. `object->update(dt)`).
4. **Polymorphism & Virtual Functions**: `Level` stores enemies as `std::vector<Enemy*>`. Calling `enemy->update(dt)` polymorphically executes `SecurityBot::update(dt)`.
5. **Composition**: `Level` composes `Player`, `SecurityBot`, `Collectible`, and `Door` objects into a cohesive world.
6. **Encapsulation**: Private members with public getters/setters (`getHealth()`, `hasKeycard()`).
7. **Resource Management (RAII)**: `Level` destructor cleanly deletes dynamically allocated game objects (`delete player`, `delete door`, etc.).

---

## Game Loop Explanation

The core game loop resides in `Game::run()` and follows a strict 3-phase cycle:

```cpp
while (running) {
    handleEvents(running); // 1. Collect player input & SDL events
    update(dt);            // 2. Advance game simulation & state by dt
    render();              // 3. Draw current state to screen
}
```

* **`handleEvents()`**: Polls SDL events, handles window quit / state transitions, and updates `InputHandler`.
* **`update(dt)`**: Delegates physics, AI state machine ticks, collision resolution, and power drain to `Level`.
* **`render()`**: Clears the renderer, draws rooms, entities, particles, HUD, or overlay screens (Menu/GameOver/Victory), and calls `SDL_RenderPresent()`.

---

## Enemy AI & State Machine

`SecurityBot` uses a 3-state Finite State Machine (FSM) defined by `enum class EnemyState`:

* **`PATROL`**: Moves back and forth along a fixed horizontal range (`patrolMin` to `patrolMax`).
* **`CHASE`**: Switches to chase mode when player distance is `< DETECT_RANGE` (200px). Moves directly toward player at elevated speed.
* **`ATTACK`**: Enters attack mode when player distance is `< ATTACK_RANGE` (35px). Stands still and deals continuous damage to the player.

---

## Collision System

Collision detection uses Axis-Aligned Bounding Box (**AABB**) rectangle overlap checks via the helper function `checkCollision()` in `include/draw.h`:

```cpp
inline bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x &&
           a.y < b.y + b.h && a.y + a.h > b.y;
}
```

Used for:
* **Player ↔ Wall/Room bounds**: Keeps player inside valid room rectangles.
* **Player ↔ Collectibles**: Triggers keycard pickup or battery power boost.
* **Player ↔ Enemies**: Triggers damage and hit particles.
* **Player ↔ Door**: Unlocks door if player possesses the keycard.
* **Player ↔ Escape Pod**: Triggers victory condition.
