#pragma once
#include <gl2d/gl2d.h>
#include <glm/vec2.hpp>

constexpr float playerShipSize = 250.f;

// ============================================================
// Player class  --  OOP Design
// Encapsulates the player spaceship state and behaviour.
// ============================================================
class Player
{
public:
    glm::vec2 position  = {100.f, 100.f};
    glm::vec2 mouseDir  = {1.f, 0.f};   // direction toward mouse cursor
    float     health    = 1.f;
    float     speed     = 2000.f;

    // -- Constructor --
    Player() = default;

    // -- Interface --
    void handleInput(float deltaTime, int windowW, int windowH);
    void render(gl2d::Renderer2D& renderer,
                gl2d::Texture& sprites,
                gl2d::TextureAtlasPadding& atlas) const;

    void takeDamage(float amount) { health -= amount; }
    void regenHealth(float deltaTime) { health += deltaTime * 0.05f; if (health > 1.f) health = 1.f; }
    bool isDead() const { return health <= 0.f; }
    void reset() { position = {100.f, 100.f}; health = 1.f; }
};
