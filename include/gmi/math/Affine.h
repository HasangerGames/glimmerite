#pragma once

#include "Transform.h"
#include "Vec2.h"
#include <iostream>

namespace gmi::math {

/**
 * A fast matrix for 2D transformations, supporting position, rotation, scale, and shear.
 * Heavily based on Pixi.js' Matrix.
 * Represents a 3x3 transformation matrix:
 *
 * ```
 * | a  c  x |
 * | b  d  y |
 * | 0  0  1 |
 * ```
 */
struct Affine {
    /** Scale on the x axis. */
    float a{1};

    /** Shear on the y axis. */
    float b{0};

    /** Shear on the x axis. */
    float c{0};

    /** Scale on the y axis. */
    float d{1};

    /** Translation on the x axis. */
    float x{0};

    /** Translation on the y axis. */
    float y{0};

    /**
     * Creates an Affine from a Transform.
     * @param t A transform
     * @return The affine equivalent
     */
    static Affine fromTransform(const Transform& t) {
        auto [position, size, rotation, scale, pivot, _]{t};
        Affine m;

        const float c{std::cos(rotation)};
        const float s{std::sin(rotation)};

        m.a = c * scale.x;
        m.b = s * scale.x;
        m.c = -s * scale.y;
        m.d = c * scale.y;

        m.x = position.x + pivot.x - ((pivot.x * m.a) + (pivot.y * m.c));
        m.y = position.y + pivot.y - ((pivot.x * m.b) + (pivot.y * m.d));

        return m;
    }
};

inline std::ostream& operator<<(std::ostream& stream, const Affine& m) {
    stream << "Affine" << std::endl << "|" << m.a << " " << m.c << " " << m.x << "|" << std::endl << "|" << m.b << " " << m.d << " " << m.y << "|" << std::endl;
    return stream;
}

inline bool operator==(const Affine& a, const Affine& b) {
    return a.a == b.a && a.b == b.b
        && a.c == b.c && a.d == b.d
        && a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Affine& a, const Affine& b) {
    return !(a == b);
}

inline Affine operator*(const Affine& a, const Affine& b) {
    Affine m;

    m.a = a.a * b.a + a.c * b.b;
    m.b = a.b * b.a + a.d * b.b;
    m.c = a.a * b.c + a.c * b.d;
    m.d = a.b * b.c + a.d * b.d;

    m.x = a.a * b.x + a.c * b.y + a.x;
    m.y = a.b * b.x + a.d * b.y + a.y;

    return m;
}

/**
 * Applies an Affine transformation to a Vec2.
 * @param m The affine to apply
 * @param pos The vector to apply it to
 * @return The transformed vector
 */
inline Vec2 affineApply(const Affine& m, const Vec2& pos) {
    auto [x, y]{pos};
    return {
        .x = (m.a * x) + (m.c * y) + m.x,
        .y = (m.b * x) + (m.d * y) + m.y
    };
}

/**
 * Inverts an Affine transformation applied to a Vec2.
 * @param m The affine to reverse
 * @param pos The vector to apply it to
 * @return The transformed vector
 */
inline Vec2 affineApplyInverse(const Affine& m, const Vec2& pos) {
    const float a{m.a};
    const float b{m.b};
    const float c{m.c};
    const float d{m.d};
    const float tx{m.x};
    const float ty{m.y};

    const float id{1 / ((a * d) + (c * -b))};

    const float x{pos.x};
    const float y{pos.y};

    return {
        .x = (d * id * x) + (-c * id * y) + (((ty * c) - (tx * d)) * id),
        .y = (a * id * y) + (-b * id * x) + (((-ty * a) + (tx * b)) * id)
    };
}

}
