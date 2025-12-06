#include "gmi/Sprite.h"

namespace gmi {

Sprite::Sprite(Application *app, const std::string &textureName, const math::Transform &transform) :
    Container(app),
    m_texture(app->texture().get(textureName)) {
    m_transform = transform;
    m_transformDirty = true;
}


Sprite::~Sprite() = default;

const std::vector<math::Vertex> QUAD_VERTS{
    {0.0f, 1.0f, 0.0f, 1.0f, 0xffffffff}, // Top left
    {0.0f, 0.0f, 0.0f, 0.0f, 0xffffffff}, // Bottom left
    {1.0f, 1.0f, 1.0f, 1.0f, 0xffffffff}, // Top right
    {0.0f, 0.0f, 0.0f, 0.0f, 0xffffffff}, // Bottom left
    {1.0f, 0.0f, 1.0f, 0.0f, 0xffffffff}, // Bottom right
    {1.0f, 1.0f, 1.0f, 1.0f, 0xffffffff}, // Top right
};

void Sprite::updateAffine() {
    Container::updateAffine();
    const math::Affine affineScaled = m_affine * math::Affine::scaleAbout(
        m_transform.pivot,
        math::Vec2::fromInt(m_texture.width, m_texture.height)
    );
    m_drawable = {
        math::transformVertices(QUAD_VERTS, affineScaled),
        &m_texture
    };
}

void Sprite::render(Renderer& renderer) {
    Container::render(renderer);
    renderer.queueDrawable(m_drawable);
}

}
