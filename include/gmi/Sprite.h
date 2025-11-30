#pragma once
#include "Container.h"

namespace gmi {

class Sprite final : public Container {
    math::Affine m_affineScaled;
    Texture& m_texture;
public:
    explicit Sprite(Texture& texture) : m_texture(texture) { Sprite::updateAffine(); }
    Sprite(Texture& texture, const math::Transform& transform) : m_texture(texture) { setTransform(transform); }
    ~Sprite() override;

    void updateAffine() override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Backend &backend) const override;
};

}
