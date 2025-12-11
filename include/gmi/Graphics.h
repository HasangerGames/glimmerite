#pragma once

#include "Container.h"
#include "gmi/math/Rect.h"
#include "mapbox/earcut.hpp"

namespace gmi {

struct StrokeStyle {
    uint16_t width;
    Color color = {1.0f, 1.0f, 1.0f, 1.0f};
};

class Graphics : public Container {
    Drawable m_drawable;

public:
    Graphics(Application* parentApp, Container* parent) : Container(parentApp, parent) { }

    Graphics& fillRect(const math::Rect& rect, Color color);

    Graphics& fillPoly(const std::vector<math::Vec2>& points, Color color);

    void render(Renderer& renderer) override;
};

}

// Makes earcut understand Vec2
namespace mapbox::util {

template<>
struct nth<0, gmi::math::Vec2> {
    static auto get(const gmi::math::Vec2& t) {
        return t.x;
    }
};
template<>
struct nth<1, gmi::math::Vec2> {
    static auto get(const gmi::math::Vec2& t) {
        return t.y;
    }
};

}
