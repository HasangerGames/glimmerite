#pragma once
#include "Container.h"

namespace gmi {

class Sprite final : public Container {
    Drawable m_drawable;
    Texture& m_texture;
public:
    explicit Sprite(Texture& texture) : m_texture(texture) {}
    Sprite(Texture& texture, const math::Transform& transform) : m_texture(texture) {
        m_transform = transform;
        m_transformDirty = true;
    }
    ~Sprite() override;

    void updateAffine() override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Backend &backend) override;
};

}
