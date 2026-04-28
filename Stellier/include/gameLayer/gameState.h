#pragma once
#include <player.h>
#include <bullet.h>
#include <enemy.h>
#include <vector>

// ============================================================
// GameState  --  OOP Design
// Central data container for the entire game session.
// ============================================================
struct GameState
{
    Player             player;
    std::vector<Bullet> bullets;
    std::vector<Enemy>  enemies;
    float spawnTimer = 3.f;

    void reset()
    {
        player.reset();
        bullets.clear();
        enemies.clear();
        spawnTimer = 3.f;
    }
};
