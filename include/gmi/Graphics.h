#pragma once

#include "Container.h"
#include "gmi/math/Rect.h"
#include "mapbox/earcut.hpp"

namespace gmi {

enum class LineCap : uint8_t {
    Butt,
    Round,
    Square
};

enum class LineJoin : uint8_t {
    Miter,
    Round,
    Bevel
};

struct StrokeStyle {
    float width = 1;
    Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    LineCap cap = LineCap::Square;
    LineJoin join = LineJoin::Miter;
    float alignment = 0.5f;
    bool closedShape = false;
    uint8_t miterLimit = 10;
};

class Graphics : public Container {
    Drawable m_drawable;

public:
    Graphics(Application* parentApp, Container* parent) : Container(parentApp, parent) { }

    Graphics& drawLine(std::vector<math::Vec2> points, const StrokeStyle& style);

    Graphics& fillRect(const math::Rect& rect, Color color);

    Graphics& fillCircle(math::Vec2 position, float rx, float ry, Color color);

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
