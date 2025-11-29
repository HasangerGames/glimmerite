#pragma once
#include "Container.h"

namespace gmi {

class Sprite final : public Container {
    Texture& m_texture;
public:
    explicit Sprite(Texture& texture) : m_texture(texture) {}
    Sprite(Texture& texture, const math::Transform& transform) : m_texture(texture) { setTransform(transform); }
    ~Sprite() override;

    void setTransform(const math::Transform& transform) override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Backend &backend, const math::Affine& affine) const override;
};

}
