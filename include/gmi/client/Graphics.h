#pragma once

#include "gmi/client/Container.h"

namespace gmi {

// avoid including the collision headers here for faster compile times :)
namespace collision {
class Shape;
}

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
    Color color = Color::White;
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

    Graphics& clear();

    Graphics& drawLine(std::vector<math::Vec2f> points, const StrokeStyle& style);

    Graphics& fillRect(float x, float y, float w, float h, Color color);

    Graphics& fillCircle(float x, float y, float r, Color color);

    Graphics& fillEllipse(float x, float y, float rx, float ry, Color color);

    Graphics& fillPoly(const std::vector<math::Vec2f>& points, Color color);

    Graphics& fillShape(const collision::Shape& shape, Color color);

    void render(Renderer& renderer) override;
};

}
