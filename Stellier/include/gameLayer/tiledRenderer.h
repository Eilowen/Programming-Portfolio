#pragma once
#include <gl2d/gl2d.h>

// ============================================================
// TiledRenderer  --  renders a looping parallax background
// ============================================================
class TiledRenderer
{
public:
    gl2d::Texture texture;
    float paralaxStrength  = 0.f;
    float backgroundSize   = 1000.f;

    void render(gl2d::Renderer2D& renderer) const;
};

// Free function used by both Player and Enemy rendering
void renderSpaceShip(gl2d::Renderer2D& renderer,
                     glm::vec2 position, float size,
                     gl2d::Texture texture, glm::vec4 uvs,
                     glm::vec2 viewDirection);
