#include <tiledRenderer.h>
#include <glm/glm.hpp>

// ============================================================
// TiledRenderer::render
// Tiles the background texture with parallax offset.
// ============================================================
void TiledRenderer::render(gl2d::Renderer2D& renderer) const
{
    auto viewRect = renderer.getViewRect();

    glm::vec2 paralaxOffset = glm::vec2(viewRect.x, viewRect.y) * (-paralaxStrength);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            int posX = x + static_cast<int>((viewRect.x - paralaxOffset.x) / backgroundSize);
            int posY = y + static_cast<int>((viewRect.y - paralaxOffset.y) / backgroundSize);

            renderer.renderRectangle(
                glm::vec4{posX, posY, 1, 1} * backgroundSize
                    + glm::vec4(paralaxOffset, 0, 0),
                texture);
        }
    }
}

// ============================================================
// renderSpaceShip  (free helper used by Player and Enemy)
// ============================================================
void renderSpaceShip(gl2d::Renderer2D& renderer,
                     glm::vec2 position, float size,
                     gl2d::Texture texture, glm::vec4 uvs,
                     glm::vec2 viewDirection)
{
    float angle = glm::degrees(atan2(viewDirection.y, -viewDirection.x)) + 90.f;

    renderer.renderRectangle(
        {position - glm::vec2(size / 2.f, size / 2.f), size, size},
        texture, Colors_White, {}, angle, uvs);
}
