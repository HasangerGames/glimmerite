#include "gmi/Sprite.h"

namespace gmi {

Sprite::~Sprite() = default;

void Sprite::setTransform(const math::Transform &transform) {
    m_transform = transform;
    m_affine = math::transformToAffine(m_transform);
}


const std::vector<math::Vertex> QUAD_VERTS{
    {0.0f, 1.0f, 0.0f, 1.0f}, // Top left
    {0.0f, 0.0f, 0.0f, 0.0f}, // Bottom left
    {1.0f, 1.0f, 1.0f, 1.0f}, // Top right
    {0.0f, 0.0f, 0.0f, 0.0f}, // Bottom left
    {1.0f, 0.0f, 1.0f, 0.0f}, // Bottom right
    {1.0f, 1.0f, 1.0f, 1.0f}, // Top right
};

void Sprite::render(Backend& backend, const math::Affine& affine) const {
    Container::render(backend, affine);
    const math::Affine scaled = math::affineScale(m_affine * affine, math::Vec2::fromInt(m_texture.getWidth(), m_texture.getHeight()));
    backend.queueGeometry({
        scaled,
        {},
        &m_texture,
        QUAD_VERTS
    });
}

}
