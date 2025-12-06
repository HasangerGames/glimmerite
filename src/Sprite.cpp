#include "gmi/Sprite.h"

namespace gmi {

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
        math::Vec2::fromInt(m_texture.getWidth(), m_texture.getHeight())
    );
    m_drawable = {
        math::transformVertices(QUAD_VERTS, affineScaled),
        &m_texture
    };
}

void Sprite::render(Backend& backend) {
    Container::render(backend);
    backend.queueDrawable(m_drawable);
}

}
