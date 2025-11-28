#pragma once
#include "Vec2.h"

namespace gmi::math {

struct Transform {
    Vec2 position;
    float rotation{0};
    Vec2 pivot{0.5f, 0.5f};
    Vec2 scale{1, 1};
};

inline std::ostream& operator<<(std::ostream& stream, const Transform& t) {
    stream << "Transform{position: " << t.position << ", rotation: " << t.rotation << ", pivot: " << t.pivot << ", scale: " << t.scale << "}";
    return stream;
}

inline Transform combineTransforms(const Transform& a, const Transform& b) {
    return {
        a.position + b.position,
        normalizeAngle(a.rotation + b.rotation),
        b.pivot,
        a.scale * b.scale
    };
}

/**
 * Applies a Transform to a @ref Vec2.
 * @param v The vector to apply the transform to
 * @param t The transform to apply
 * @return The transformed vector
 */
inline Vec2 applyTransform(const Vec2& v, const Transform& t) {
    const Vec2 scaled{(v - t.pivot) * t.scale};
    return t.position + rotate(scaled, t.rotation);
}

}
