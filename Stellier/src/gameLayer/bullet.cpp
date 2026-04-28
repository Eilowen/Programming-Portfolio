#include <bullet.h>
#include <glm/glm.hpp>

// ============================================================
// Bullet::render
// Draws 5 layered sprites for a motion-trail effect.
// ============================================================
void Bullet::render(gl2d::Renderer2D& renderer,
                    gl2d::Texture bulletsTexture,
                    gl2d::TextureAtlasPadding bulletsAtlas) const
{
    float angle = glm::degrees(atan2(fireDirection.y, -fireDirection.x)) + 90.f;

    glm::vec4 textureCoords = isEnemy
        ? bulletsAtlas.get(0, 0)
        : bulletsAtlas.get(1, 1);

    for (int i = 0; i < 5; i++)
    {
        float alpha = static_cast<float>(i + 1) / 5.f;
        float bright = static_cast<float>(i + 4) / 5.f;
        glm::vec4 color(bright, bright, bright, alpha);

        renderer.renderRectangle(
            {position - glm::vec2(50, 50) + static_cast<float>(i) * 25.f * fireDirection, 100, 100},
            bulletsTexture, color, {}, angle, textureCoords);
    }
}

// ============================================================
// Bullet::update
// Moves the bullet along its fire direction each frame.
// ============================================================
void Bullet::update(float deltaTime)
{
    position += fireDirection * deltaTime * speed;
}
