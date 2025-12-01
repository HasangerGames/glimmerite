#pragma once
#include "Container.h"

namespace gmi {

class Sprite final : public Container {
    math::Geometry m_geometry;
    Texture& m_texture;
public:
    explicit Sprite(Texture& texture) : m_texture(texture) { Sprite::updateAffine(); }
    Sprite(Texture& texture, const math::Transform& transform) : m_texture(texture) {
        m_transform = transform;
        updateAffine();
    }
    ~Sprite() override;

    void updateAffine() override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Backend &backend) const override;
};

}
