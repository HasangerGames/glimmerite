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

    auto& [handle, layer, size, frame, scale, _] = m_texture;

    auto tw = (float) size.w;
    auto th = (float) size.h;
    auto fx = (float) frame.x;
    auto fy = (float) frame.y;
    auto fw = (float) frame.w;
    auto fh = (float) frame.h;
    auto l  = (float) layer;

    auto [a, b, c, d, x, y, color] = m_affine * math::Affine::scaleAbout(m_transform.pivot, {fw * scale, fh * scale});

    // clang-format off
    float lx = fx / tw;        // left X
    float rx = (fx + fw) / tw; // right X
    float ty = fy / th;        // top Y
    float by = (fy + fh) / th; // bottom Y

    m_drawable = {
        .vertices = {
            {c + x,     d + y,     lx, by + l, color}, // Top left
            {a + c + x, b + d + y, rx, by + l, color}, // Top right
            {a + x,     b + y,     rx, ty + l, color}, // Bottom right
            {x,         y,         lx, ty + l, color}, // Bottom left
        },
    // clang-format on
        .indices = {0, 1, 2, 0, 2, 3},
        .texture = handle,
    };
}

void Sprite::render(Renderer& renderer) {
    Container::render(renderer);

    if (m_visible) {
        renderer.queueDrawable(m_drawable);
    }
}

}
