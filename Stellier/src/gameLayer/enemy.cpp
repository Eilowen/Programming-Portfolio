#include <enemy.h>
#include <tiledRenderer.h>
#include <glm/glm.hpp>
#include <cstdlib>

// ============================================================
// Enemy::render
// ============================================================
void Enemy::render(gl2d::Renderer2D& renderer,
                   gl2d::Texture& sprites,
                   gl2d::TextureAtlasPadding& atlas) const
{
    renderSpaceShip(renderer, position, enemyShipSize,
                    sprites, atlas.get(type.x, type.y), viewDirection);
}

// ============================================================
// Enemy::update
// Steers toward the player and fires when facing them.
// Returns true when the enemy should fire a bullet.
// ============================================================
bool Enemy::update(float deltaTime, glm::vec2 playerPos)
{
    // --- Compute direction to player ---
    glm::vec2 directionToPlayer = playerPos - position;
    if (glm::length(directionToPlayer) == 0.f)
        directionToPlayer = {1, 0};
    else
        directionToPlayer = glm::normalize(directionToPlayer);

    // --- Determine whether to fire ---
    bool wantsToShoot = (glm::length(directionToPlayer + viewDirection) >= fireRange);
    bool actuallyShoot = false;

    if (wantsToShoot)
    {
        if (firedTime <= 0.f)
        {
            firedTime     = fireTimeReset;
            actuallyShoot = true;
        }
    }

    firedTime -= deltaTime;
    if (firedTime < 0.f) firedTime = 0.f;

    // --- Steering ---
    glm::vec2 newDirection;
    if (glm::length(directionToPlayer + viewDirection) <= 0.2f)
    {
        // Perpendicular dodge when almost directly behind player
        newDirection = (rand() % 2)
            ? glm::vec2( directionToPlayer.y, -directionToPlayer.x)
            : glm::vec2(-directionToPlayer.y,  directionToPlayer.x);
    }
    else
    {
        newDirection = deltaTime * turnSpeed * directionToPlayer + viewDirection;
    }

    float len    = glm::clamp(glm::length(newDirection), 0.1f, 3.f);
    viewDirection = glm::normalize(newDirection);
    position    += viewDirection * deltaTime * speed * len;

    return actuallyShoot;
}
