#include "gmi/Sprite.h"

namespace gmi {

Sprite::~Sprite() = default;

const std::vector<math::Vertex> QUAD_VERTS{
    {0.0f, 1.0f, 0.0f, 1.0f}, // Top left
    {0.0f, 0.0f, 0.0f, 0.0f}, // Bottom left
    {1.0f, 1.0f, 1.0f, 1.0f}, // Top right
    {0.0f, 0.0f, 0.0f, 0.0f}, // Bottom left
    {1.0f, 0.0f, 1.0f, 0.0f}, // Bottom right
    {1.0f, 1.0f, 1.0f, 1.0f}, // Top right
};

void Sprite::updateAffine() {
    Container::updateAffine();
    const math::Affine affineScaled = m_affine * math::Affine::scaleAbout(
        m_transform.pivot,
        math::Vec2::fromInt(m_texture.getWidth(), m_texture.getHeight())
    );
    m_geometry = {
        affineScaled,
        {},
        &m_texture,
        &QUAD_VERTS
    };
}

void Sprite::render(Backend& backend) const {
    Container::render(backend);
    backend.queueGeometry(m_geometry);
}

}
