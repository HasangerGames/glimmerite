#pragma once
#include "Vec2.h"
#include "gmi/Color.h"

namespace gmi::math {

struct Transform {
    Vec2 position;
    float rotation = 0;
    Vec2 scale = {1, 1};
    Vec2 pivot = {0.5f, 0.5f};
    Color color;
};

enum class TransformProps {
    Position,
    Rotation,
    Scale,
    Pivot,
    Color
};

inline std::ostream& operator<<(std::ostream& stream, const Transform& t) {
    stream << "Transform{position: " << t.position << ", rotation: " << t.rotation
           << ", pivot: " << t.pivot << ", scale: " << t.scale << ", color: " << t.color << "}";
    return stream;
}

}
