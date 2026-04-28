#pragma once
#include <gl2d/gl2d.h>

constexpr float enemyShipSize = 250.f;

// ============================================================
// Enemy class  --  OOP Design
// Represents a single enemy spaceship with AI behaviour.
// ============================================================
class Enemy
{
public:
    // --- Data ---
    glm::uvec2 type          = {};
    glm::vec2  position      = {};
    glm::vec2  viewDirection = {1, 0};

    float speed         = 1500.f;
    float turnSpeed     = 3.f;
    float firedTime     = 1.f;
    float fireTimeReset = 0.2f;
    float fireRange     = 1.5f;
    float bulletSpeed   = 2000.f;
    float life          = 1.f;

    // --- Constructor ---
    Enemy() = default;

    // --- Interface ---
    void render(gl2d::Renderer2D& renderer,
                gl2d::Texture& sprites,
                gl2d::TextureAtlasPadding& atlas) const;

    // Returns true if the enemy fires a bullet this frame
    bool update(float deltaTime, glm::vec2 playerPos);

    bool isDead() const { return life <= 0.f; }
    void takeDamage(float amount) { life -= amount; }
};
