// ============================================================
// gameLayer.cpp  --  Space Shooter (OOP version)
// Features: Main Menu, Game Over screen, Pause, HUD
// ============================================================

#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <tiledRenderer.h>
#include <bullet.h>
#include <enemy.h>
#include <player.h>
#include <gameState.h>
#include <glui/glui.h>
#include <raudio.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// ============================================================
// Game screen states
// ============================================================
enum class Screen { MENU, PLAYING, PAUSED, GAMEOVER };

// ============================================================
// Module-level singletons
// ============================================================
static gl2d::Renderer2D  renderer;
static GameState         gameState;
static Screen            currentScreen = Screen::MENU;
static int               score         = 0;
static int               highScore     = 0;

static constexpr int BACKGROUNDS = 4;
static gl2d::Texture             spaceShipsTexture;
static gl2d::TextureAtlasPadding spaceShipsAtlas;
static gl2d::Texture             bulletsTexture;
static gl2d::TextureAtlasPadding bulletsAtlas;
static gl2d::Texture             backgroundTexture[BACKGROUNDS];
static TiledRenderer             tiledRenderer[BACKGROUNDS];
static gl2d::Texture             healthBarTex;
static gl2d::Texture             healthTex;
static gl2d::Font                font;
static Sound                     shootSound;

// ============================================================
// Helpers
// ============================================================
static bool bulletHitsCircle(glm::vec2 bp, glm::vec2 center, float r)
{
    return glm::distance(bp, center) <= r;
}

static void spawnEnemy()
{
    static const glm::uvec2 shipTypes[] = {{0,0},{0,1},{2,0},{3,1}};
    Enemy e;
    e.position = gameState.player.position;
    glm::vec2 offset(2000.f, 0.f);
    float angle = glm::radians(static_cast<float>(rand() % 360));
    offset = glm::vec2(glm::vec4(offset,0,1) *
        glm::rotate(glm::mat4(1.f), angle, glm::vec3(0,0,1)));
    e.position     += offset;
    e.speed         = 800.f  + rand() % 1000;
    e.turnSpeed     = 2.2f   + (rand() % 1000) / 500.f;
    e.type          = shipTypes[rand() % 4];
    e.fireRange     = 1.5f   + (rand() % 1000) / 2000.f;
    e.fireTimeReset = 0.1f   + (rand() % 1000) / 500.f;
    e.bulletSpeed   = static_cast<float>(rand() % 3000 + 1000);
    gameState.enemies.push_back(e);
}

static void startGame()
{
    score = 0;
    gameState.reset();
    renderer.currentCamera.follow(
        gameState.player.position, 550, 0, 0,
        renderer.windowW, renderer.windowH);
    currentScreen = Screen::PLAYING;
}

// ============================================================
// Render a centered text button, returns true if clicked
// ============================================================
static bool menuButton(gl2d::Renderer2D& r, gl2d::Font& f,
                       const char* label,
                       float cx, float cy, float w, float h,
                       int mouseX, int mouseY, bool clicked,
                       glm::vec4 col = {0.15f,0.15f,0.35f,0.92f},
                       glm::vec4 hoverCol = {0.25f,0.25f,0.6f,0.95f})
{
    glm::vec4 rect = {cx - w/2.f, cy - h/2.f, w, h};
    bool hovered = mouseX >= rect.x && mouseX <= rect.x + rect.z
                && mouseY >= rect.y && mouseY <= rect.y + rect.w;

    // Shadow
    glm::vec4 shadow = {rect.x+4, rect.y+4, rect.z, rect.w};
    r.renderRectangle(shadow, {0,0,0,0.5f});
    // Button body
    r.renderRectangle(rect, hovered ? hoverCol : col);
    // Border
    r.renderRectangle({rect.x,   rect.y,   rect.z, 2}, {0.5f,0.7f,1.f,1.f});
    r.renderRectangle({rect.x,   rect.y+rect.w-2, rect.z, 2}, {0.5f,0.7f,1.f,1.f});
    r.renderRectangle({rect.x,   rect.y,   2, rect.w}, {0.5f,0.7f,1.f,1.f});
    r.renderRectangle({rect.x+rect.z-2, rect.y, 2, rect.w}, {0.5f,0.7f,1.f,1.f});
    // Label
    r.renderText({cx, cy}, label, f, Colors_White, 0.55f, true);

    return hovered && clicked;
}

// ============================================================
// Render the scrolling background (used on all screens)
// ============================================================
static void renderBackground()
{
    renderer.currentCamera.zoom = 0.5f;
    for (int i = 0; i < BACKGROUNDS; i++)
        tiledRenderer[i].render(renderer);
}

// ============================================================
// MENU SCREEN
// ============================================================
static void renderMenu(int w, int h, int mx, int my, bool clicked)
{
    float cx = w / 2.f;

    // Title
    renderer.renderText({cx, h * 0.22f}, "SPACE SHOOTER", font,
                        {0.4f,0.8f,1.f,1.f}, 1.4f, true);
    renderer.renderText({cx, h * 0.33f}, "OOP Edition", font,
                        {0.7f,0.7f,1.f,0.8f}, 0.55f, true);

    // Subtitle / instructions hint
    renderer.renderText({cx, h * 0.42f},
        "WASD / Arrows to move   |   Mouse to aim   |   Left Click to shoot",
        font, {0.8f,0.8f,0.8f,0.7f}, 0.32f, true);

    float bw = 280.f, bh = 60.f;

    if (menuButton(renderer, font, "PLAY",
                   cx, h*0.55f, bw, bh, mx, my, clicked))
        startGame();

    if (menuButton(renderer, font, "FULLSCREEN",
                   cx, h*0.65f, bw, bh, mx, my, clicked))
        platform::setFullScreen(!platform::isFullScreen());

    if (menuButton(renderer, font, "QUIT",
                   cx, h*0.75f, bw, bh, mx, my, clicked,
                   {0.35f,0.1f,0.1f,0.92f},{0.6f,0.15f,0.15f,0.95f}))
        exit(0);

    // High score
    char buf[64];
    std::snprintf(buf, sizeof(buf), "High Score: %d", highScore);
    renderer.renderText({cx, h*0.88f}, buf, font,
                        {1.f,0.85f,0.3f,0.9f}, 0.42f, true);
}

// ============================================================
// PAUSE SCREEN overlay
// ============================================================
static void renderPause(int w, int h, int mx, int my, bool clicked)
{
    // Dim overlay
    renderer.renderRectangle({0,0,(float)w,(float)h}, {0,0,0,0.55f});

    float cx = w/2.f;
    renderer.renderText({cx, h*0.3f}, "PAUSED", font,
                        {1.f,1.f,1.f,1.f}, 1.1f, true);

    float bw = 260.f, bh = 56.f;

    if (menuButton(renderer, font, "RESUME",
                   cx, h*0.48f, bw, bh, mx, my, clicked))
        currentScreen = Screen::PLAYING;

    if (menuButton(renderer, font, "MAIN MENU",
                   cx, h*0.58f, bw, bh, mx, my, clicked))
        currentScreen = Screen::MENU;

    if (menuButton(renderer, font, "QUIT",
                   cx, h*0.68f, bw, bh, mx, my, clicked,
                   {0.35f,0.1f,0.1f,0.92f},{0.6f,0.15f,0.15f,0.95f}))
        exit(0);
}

// ============================================================
// GAME OVER SCREEN
// ============================================================
static void renderGameOver(int w, int h, int mx, int my, bool clicked)
{
    renderer.renderRectangle({0,0,(float)w,(float)h}, {0,0,0,0.6f});

    float cx = w/2.f;
    renderer.renderText({cx, h*0.28f}, "GAME OVER", font,
                        {1.f,0.25f,0.25f,1.f}, 1.2f, true);

    char buf[64];
    std::snprintf(buf, sizeof(buf), "Score: %d", score);
    renderer.renderText({cx, h*0.4f}, buf, font,
                        {1.f,1.f,1.f,1.f}, 0.65f, true);

    std::snprintf(buf, sizeof(buf), "High Score: %d", highScore);
    renderer.renderText({cx, h*0.48f}, buf, font,
                        {1.f,0.85f,0.3f,1.f}, 0.5f, true);

    float bw = 260.f, bh = 56.f;

    if (menuButton(renderer, font, "PLAY AGAIN",
                   cx, h*0.60f, bw, bh, mx, my, clicked))
        startGame();

    if (menuButton(renderer, font, "MAIN MENU",
                   cx, h*0.70f, bw, bh, mx, my, clicked))
        currentScreen = Screen::MENU;
}

// ============================================================
// HUD (health bar + score) drawn in screen space
// ============================================================
static void renderHUD(int w, int h)
{
    renderer.pushCamera();
    {
        glui::Frame f({0, 0, w, h});

        // Health bar background
        glui::Box healthBox = glui::Box()
            .xLeftPerc(0.02f)
            .yTopPerc(0.04f)
            .xDimensionPercentage(0.28f)
            .yAspectRatio(1.f / 7.f);

        renderer.renderRectangle(healthBox, healthBarTex);

        // Health fill
        glm::vec4 filledRect = healthBox();
        filledRect.z *= gameState.player.health;
        glm::vec4 texCoords = {0, 1, 1, 0};
        texCoords.z *= gameState.player.health;
        renderer.renderRectangle(filledRect, healthTex,
                                 Colors_White, {}, {}, texCoords);

        // "HP" label
        renderer.renderText({filledRect.x + healthBox().z / 2.f,
                              filledRect.y + healthBox().w / 2.f},
                             "HP", font, Colors_White, 0.5f, true);

        // Score
        char buf[32];
        std::snprintf(buf, sizeof(buf), "Score: %d", score);
        renderer.renderText({(float)w - 200.f, 28.f}, buf, font,
                            {1.f,0.9f,0.3f,1.f}, 0.38f, false);

        // ESC hint
        renderer.renderText({(float)w/2.f, (float)h - 24.f},
                            "ESC = Pause", font,
                            {1.f,1.f,1.f,0.45f}, 0.4f, true);
    }
    renderer.popCamera();
}

// ============================================================
// initGame
// ============================================================
bool initGame()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    gl2d::init();
    renderer.create();

    spaceShipsTexture.loadFromFileWithPixelPadding(
        RESOURCES_PATH "spaceShip/stitchedFiles/spaceships.png", 128, true);
    spaceShipsAtlas = gl2d::TextureAtlasPadding(5, 2,
        spaceShipsTexture.GetSize().x, spaceShipsTexture.GetSize().y);

    bulletsTexture.loadFromFileWithPixelPadding(
        RESOURCES_PATH "spaceShip/stitchedFiles/projectiles.png", 500, true);
    bulletsAtlas = gl2d::TextureAtlasPadding(3, 2,
        bulletsTexture.GetSize().x, bulletsTexture.GetSize().y);

    healthBarTex.loadFromFile(RESOURCES_PATH "healthBar.png", true);
    healthTex.loadFromFile(RESOURCES_PATH "health.png", true);

    font.createFromFile(RESOURCES_PATH "CommodorePixeled.ttf");

    shootSound = LoadSound(RESOURCES_PATH "shoot.flac");
    SetSoundVolume(shootSound, 0.1f);

    for (int i = 0; i < BACKGROUNDS; i++)
    {
        char path[256];
        std::snprintf(path, sizeof(path), RESOURCES_PATH "background%d.png", i+1);
        backgroundTexture[i].loadFromFile(path, true);
        tiledRenderer[i].texture = backgroundTexture[i];
    }
    tiledRenderer[0].paralaxStrength = 0.0f;
    tiledRenderer[1].paralaxStrength = 0.2f;
    tiledRenderer[2].paralaxStrength = 0.4f;
    tiledRenderer[3].paralaxStrength = 0.7f;

    currentScreen = Screen::MENU;
    return true;
}

// ============================================================
// gameLogic  --  called every frame
// ============================================================
bool gameLogic(float deltaTime)
{
    int w = platform::getFrameBufferSizeX();
    int h = platform::getFrameBufferSizeY();
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    renderer.updateWindowMetrics(w, h);

    glm::ivec2 mouse   = platform::getRelMousePosition();
    int mx = mouse.x, my = mouse.y;
    bool clicked = platform::isLMousePressed();

    // =========================================================
    // MENU
    // =========================================================
    if (currentScreen == Screen::MENU)
    {
        // Slowly drift camera for menu background
        gameState.player.position.x += deltaTime * 40.f;
        renderer.currentCamera.follow(gameState.player.position,
            deltaTime * 60, 0, 0, w, h);

        renderBackground();
        renderer.pushCamera();
        renderMenu(w, h, mx, my, clicked);
        renderer.popCamera();
        renderer.flush();
        return true;
    }

    // =========================================================
    // PLAYING
    // =========================================================
    if (currentScreen == Screen::PLAYING)
    {
        // ESC = pause
        if (platform::isButtonHeld(platform::Button::Escape))
            currentScreen = Screen::PAUSED;

        // --- Player input ---
        gameState.player.handleInput(deltaTime, w, h);
        renderer.currentCamera.follow(gameState.player.position,
            deltaTime * 550, 1, 150, w, h);

        renderBackground();

        // --- Shoot ---
        if (platform::isLMousePressed())
        {
            Bullet b(gameState.player.position,
                     gameState.player.mouseDir, 3000.f, false);
            gameState.bullets.push_back(b);
            PlaySound(shootSound);
        }

        // --- Bullets ---
        for (int i = 0; i < (int)gameState.bullets.size(); i++)
        {
            Bullet& b = gameState.bullets[i];

            if (glm::distance(b.position, gameState.player.position) > 5000.f)
            { gameState.bullets.erase(gameState.bullets.begin()+i); i--; continue; }

            if (!b.isEnemy)
            {
                bool removed = false;
                for (int e = 0; e < (int)gameState.enemies.size(); e++)
                {
                    if (bulletHitsCircle(b.position,
                                         gameState.enemies[e].position,
                                         enemyShipSize))
                    {
                        gameState.enemies[e].takeDamage(0.1f);
                        if (gameState.enemies[e].isDead())
                        {
                            gameState.enemies.erase(gameState.enemies.begin()+e);
                            score += 100;
                            if (score > highScore) highScore = score;
                        }
                        gameState.bullets.erase(gameState.bullets.begin()+i);
                        i--; removed = true; break;
                    }
                }
                if (removed) continue;
            }
            else
            {
                if (bulletHitsCircle(b.position,
                                     gameState.player.position,
                                     playerShipSize))
                {
                    gameState.player.takeDamage(0.1f);
                    gameState.bullets.erase(gameState.bullets.begin()+i);
                    i--; continue;
                }
            }
            b.update(deltaTime);
        }

        // --- Player death ---
        if (gameState.player.isDead())
        {
            currentScreen = Screen::GAMEOVER;
        }
        else
        {
            gameState.player.regenHealth(deltaTime);
        }

        // --- Enemy spawn ---
        if ((int)gameState.enemies.size() < 15)
        {
            gameState.spawnTimer -= deltaTime;
            if (gameState.spawnTimer < 0.f)
            {
                gameState.spawnTimer = static_cast<float>(rand() % 6 + 1);
                spawnEnemy();
                if (rand() % 3 == 0) { spawnEnemy(); spawnEnemy(); }
            }
        }

        // --- Enemy update + render ---
        for (int i = 0; i < (int)gameState.enemies.size(); i++)
        {
            Enemy& e = gameState.enemies[i];
            if (glm::distance(gameState.player.position, e.position) > 4000.f)
            { gameState.enemies.erase(gameState.enemies.begin()+i); i--; continue; }

            if (e.update(deltaTime, gameState.player.position))
            {
                Bullet b(e.position, e.viewDirection, e.bulletSpeed, true);
                gameState.bullets.push_back(b);
                if (!IsSoundPlaying(shootSound)) PlaySound(shootSound);
            }
            e.render(renderer, spaceShipsTexture, spaceShipsAtlas);
        }

        // --- Player + bullets render ---
        gameState.player.render(renderer, spaceShipsTexture, spaceShipsAtlas);
        for (auto& b : gameState.bullets)
            b.render(renderer, bulletsTexture, bulletsAtlas);

        // --- HUD ---
        renderHUD(w, h);

        renderer.flush();
        return true;
    }

    // =========================================================
    // PAUSED
    // =========================================================
    if (currentScreen == Screen::PAUSED)
    {
        // Render frozen game world underneath
        renderBackground();
        for (auto& e : gameState.enemies)
            e.render(renderer, spaceShipsTexture, spaceShipsAtlas);
        gameState.player.render(renderer, spaceShipsTexture, spaceShipsAtlas);
        for (auto& b : gameState.bullets)
            b.render(renderer, bulletsTexture, bulletsAtlas);
        renderHUD(w, h);

        // Pause overlay (screen space)
        renderer.pushCamera();
        renderPause(w, h, mx, my, clicked);
        renderer.popCamera();

        renderer.flush();
        return true;
    }

    // =========================================================
    // GAME OVERssssssss
    // =========================================================
    if (currentScreen == Screen::GAMEOVER)
    {
        renderBackground();
        renderer.pushCamera();
        renderGameOver(w, h, mx, my, clicked);
        renderer.popCamera();
        renderer.flush();
        return true;
    }

    return true;
}

void closeGame() {}
