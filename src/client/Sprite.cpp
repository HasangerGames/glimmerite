#include "gmi/client/Sprite.h"

namespace gmi {

Sprite::Sprite(Application* parentApp, Container* parent, const std::string& textureName, const math::Transform& transform) :
    Container(parentApp, parent),
    m_texture(parentApp->textures().get(textureName)) {
    m_transform = transform;
    m_transformDirty = true;

    if (m_texture.loading) {
        parentApp->textures().updateOnLoad(textureName, this);
    }
}

Sprite::~Sprite() = default;

void Sprite::updateAffine() {
    Container::updateAffine();

    auto& [handle, textureSize, frame, _] = m_texture;

    auto tw = static_cast<float>(textureSize.w);
    auto th = static_cast<float>(textureSize.h);
    auto fx = static_cast<float>(frame.x);
    auto fy = static_cast<float>(frame.y);
    auto fw = static_cast<float>(frame.w);
    auto fh = static_cast<float>(frame.h);

    auto [a, b, c, d, x, y, color] = m_affine * math::Affine::scaleAbout(m_transform.pivot, {fw, fh});

    // clang-format off
    float lx = fx / tw;        // left X
    float rx = (fx + fw) / tw; // right X
    float ty = fy / th;        // top Y
    float by = (fy + fh) / th; // bottom Y

    m_drawable = {
        .vertices = {
            {c + x,     d + y,     lx, by, color}, // Top left
            {a + c + x, b + d + y, rx, by, color}, // Top right
            {a + x,     b + y,     rx, ty, color}, // Bottom right
            {x,         y,         lx, ty, color}, // Bottom left
        },
    // clang-format on
        .indices = {0, 1, 2, 0, 2, 3},
        .texture = handle
    };
}

void Sprite::render(Renderer& renderer) {
    Container::render(renderer);

    if (m_visible) {
        renderer.queueDrawable(m_drawable);
    }
}

}
