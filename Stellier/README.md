# Space Shooter — C++ OOP Lab Project

A 2D space shooter game built with **C++17**, **OpenGL (gl2d)**, **GLFW**, and **raudio**.
Based on the "Make a C++ game FULL GUIDE!" tutorial by lowlevelgamedev.

## OOP Class Design

| Class / Struct   | Responsibility |
|------------------|----------------|
| `Player`         | Player movement (WASD/arrows), mouse aiming, health, rendering |
| `Enemy`          | AI steering toward player, firing, hit detection, rendering |
| `Bullet`         | Position, direction, speed, motion-trail rendering |
| `TiledRenderer`  | Parallax scrolling background tiles |
| `GameState`      | Aggregates Player, enemies[], bullets[], spawn timer |

## How to Build (Windows — Visual Studio)

1. Install **CMake 3.16+** and **Visual Studio 2022** (with C++ workload).
2. Open the project folder in VS2022 → File → Open → CMake Project.
3. Build → Build All.
4. Run `spaceGame.exe`.

## How to Build (Linux / WSL)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./spaceGame
```

## Controls

| Key / Button | Action |
|---|---|
| W / A / S / D or Arrow Keys | Move spaceship |
| Mouse | Aim |
| Left Mouse Button | Fire bullet |

## Third-Party Libraries

| Library | Purpose |
|---|---|
| glfw-3.3.2 | Window + input |
| glad | OpenGL loader |
| gl2d | 2D sprite renderer |
| glm | Math (vectors, matrices) |
| stb_image | PNG loading |
| stb_truetype | Font loading |
| raudio | Audio (shoot sound) |
| imgui-docking | Debug UI (optional) |
| glui | In-game UI (health bar) |
| safeSave | Save system |
| profilerLib | Performance profiling |

## Credits
- Original tutorial: https://youtu.be/XOs2qynEmNE
- Space art: https://deep-fold.itch.io/space-background-generator
- Health bar: https://wolf-viciox.itch.io/health-bar
- Ships: https://lowich.itch.io/free-spaceship-sprites
