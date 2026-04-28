#include <player.h>
#include <tiledRenderer.h>
#include <platformInput.h>
#include <glm/glm.hpp>
#include <cmath>

// ============================================================
// Player::handleInput
// Reads keyboard (WASD / arrows) and mouse, updates position
// and mouseDir used for aiming.
// ============================================================
void Player::handleInput(float deltaTime, int windowW, int windowH)
{
    glm::vec2 move = {};

    if (platform::isButtonHeld(platform::Button::W) ||
        platform::isButtonHeld(platform::Button::Up))    move.y = -1.f;

    if (platform::isButtonHeld(platform::Button::S) ||
        platform::isButtonHeld(platform::Button::Down))  move.y =  1.f;

    if (platform::isButtonHeld(platform::Button::A) ||
        platform::isButtonHeld(platform::Button::Left))  move.x = -1.f;

    if (platform::isButtonHeld(platform::Button::D) ||
        platform::isButtonHeld(platform::Button::Right)) move.x =  1.f;

    if (move.x != 0.f || move.y != 0.f)
    {
        move       = glm::normalize(move) * deltaTime * speed;
        position  += move;
    }

    // --- Mouse direction ---
    glm::vec2 mousePos   = platform::getRelMousePosition();
    glm::vec2 screenCenter(windowW / 2.f, windowH / 2.f);
    glm::vec2 dir        = mousePos - screenCenter;

    mouseDir = (glm::length(dir) == 0.f) ? glm::vec2{1, 0} : glm::normalize(dir);
}

// ============================================================
// Player::render
// ============================================================
void Player::render(gl2d::Renderer2D& renderer,
                    gl2d::Texture& sprites,
                    gl2d::TextureAtlasPadding& atlas) const
{
    renderSpaceShip(renderer, position, playerShipSize,
                    sprites, atlas.get(3, 0), mouseDir);
}
