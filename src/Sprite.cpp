#include "gmi/Sprite.h"

namespace gmi {

Sprite::Sprite(Application* app, const std::string& textureName, const math::Transform& transform) :
    Container(app),
    m_texture(app->texture().get(textureName)) {
    m_transform = transform;
    m_transformDirty = true;
}

Sprite::~Sprite() = default;

void Sprite::updateAffine() {
    Container::updateAffine();

    auto& [handle, textureSize, frame] = m_texture;

    math::Affine affineScaled = m_affine * math::Affine::scaleAbout(
        m_transform.pivot,
        math::Vec2::fromAny(frame.w, frame.h)
    );

    auto tw = static_cast<float>(textureSize.w);
    auto th = static_cast<float>(textureSize.h);
    auto fx = static_cast<float>(frame.x);
    auto fy = static_cast<float>(frame.y);
    auto fw = static_cast<float>(frame.w);
    auto fh = static_cast<float>(frame.h);

    float lx = fx / tw;        // left X
    float rx = (fx + fw) / tw; // right X
    float ty = fy / th;        // top Y
    float by = (fy + fh) / th; // bottom Y

    auto [a, b, c, d, x, y, color] = affineScaled;
    uint32_t finalColor = colorToNumber(color);

    m_drawable = {
        .vertices = {
            {a + c + x, b + d + y, rx, by, finalColor}, // Top right
            {c + x,     d + y,     lx, by, finalColor}, // Top left
            {x,         y,         lx, ty, finalColor}, // Bottom left
            {a + x,     b + y,     rx, ty, finalColor}, // Bottom right
        },
        .texture = &handle
    };
}

void Sprite::render(Renderer& renderer) {
    Container::render(renderer);

    if (m_visible) {
        renderer.queueDrawable(m_drawable);
    }
}

}
