#pragma once
#include "Vec2.h"
#include "gmi/Color.h"

namespace gmi::math {

struct Transform {
    Vec2 position;
    Vec2 size;
    float rotation{0};
    Vec2 scale{1, 1};
    Vec2 pivot{0.5f, 0.5f};
    Color color;
};

inline std::ostream& operator<<(std::ostream& stream, const Transform& t) {
    stream << "Transform{position: " << t.position << ", rotation: " << t.rotation
           << ", pivot: " << t.pivot << ", scale: " << t.scale << ", color: " << t.color << "}";
    return stream;
}

}
