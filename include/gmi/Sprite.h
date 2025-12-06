#pragma once
#include "Application.h"
#include "Container.h"

namespace gmi {

class Sprite final : public Container {
    Drawable m_drawable;
    Texture& m_texture;
public:
    explicit Sprite(Application* app, const std::string& textureName, const math::Transform& transform = {});
    ~Sprite() override;

    void updateAffine() override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Renderer &renderer) override;
};

}
