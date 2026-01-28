#pragma once
#include "gmi/client/Application.h"
#include "gmi/client/Container.h"

namespace gmi {

class Sprite : public Container {
public:
    Sprite(Application* parentApp, Container* parent, const std::string& textureName, const math::Transform& transform = {});
    ~Sprite() override = default;

    void updateAffine() override;

    [[nodiscard]] Texture& getTexture() const { return m_texture; }

    void render(Renderer& renderer) override;
private:
    Drawable m_drawable;
    Texture& m_texture;
};

}
