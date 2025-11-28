#pragma once
#include "Container.h"

namespace gmi {

class Sprite final : public Container {
    Texture& m_texture;
public:
    Sprite(Texture& texture, const math::Transform& transform) : m_texture(texture) { m_transform = transform; }
    ~Sprite() override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Backend &backend, const math::Transform& transform) const override;
};

}
