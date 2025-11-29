#include "gmi/Sprite.h"

namespace gmi {

Sprite::~Sprite() = default;

math::Affine Sprite::getAffine() const {
    math::Transform t = m_transform;
    t.scale *= math::Vec2::fromInt(m_texture.getWidth(), m_texture.getHeight());
    return math::Affine::fromTransform(t);
}

const std::vector<math::Vertex> QUAD_VERTS{
    {0.0f, 1.0f, 0.0f, 1.0f}, // Top left
    {0.0f, 0.0f, 0.0f, 0.0f}, // Bottom left
    {1.0f, 1.0f, 1.0f, 1.0f}, // Top right
    {0.0f, 0.0f, 0.0f, 0.0f}, // Bottom left
    {1.0f, 0.0f, 1.0f, 0.0f}, // Bottom right
    {1.0f, 1.0f, 1.0f, 1.0f}, // Top right
};

void Sprite::render(Backend& backend) const {
    Container::render(backend);
    backend.queueGeometry({
        m_affine,
        {},
        &m_texture,
        QUAD_VERTS
    });
}

}
