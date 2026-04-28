#pragma once
#include <gl2d/gl2d.h>

// ============================================================
// Bullet class  --  OOP Design
// Represents a single projectile fired by player or enemy.
// ============================================================
class Bullet
{
public:
    // --- Data ---
    glm::vec2 position    = {};
    glm::vec2 fireDirection = {};
    float     speed       = 3000.f;
    bool      isEnemy     = false;

    // --- Constructor ---
    Bullet() = default;
    Bullet(glm::vec2 pos, glm::vec2 dir, float spd, bool enemy)
        : position(pos), fireDirection(dir), speed(spd), isEnemy(enemy) {}

    // --- Interface ---
    void render(gl2d::Renderer2D& renderer,
                gl2d::Texture bulletsTexture,
                gl2d::TextureAtlasPadding bulletsAtlas) const;

    void update(float deltaTime);
};
