#pragma once

#include "gmi/client/Color.h"
#include "gmi/math/Vec2.h"

#include <iostream>

namespace gmi::math {

struct Transform {
    Vec2f position;
    float rotation = 0;
    Vec2f scale = {1, 1};
    Vec2f pivot = {0.5f, 0.5f};
    Color color;
};

enum class TransformProps : uint8_t {
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
