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

void Sprite::render(Backend& backend, const math::Transform& transform) const {
    Container::render(backend, transform);
    math::Transform finalTransform = math::combineTransforms(m_transform, transform);
    finalTransform.scale *= math::Vec2{ m_texture.getWidth(), m_texture.getHeight() };
    backend.queueGeometry({
        finalTransform,
        &m_texture,
        QUAD_VERTS
    });
}

}
