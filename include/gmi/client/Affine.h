#pragma once

#include "gmi/client/Transform.h"
#include "gmi/math/Vec2.h"
#include <iostream>

namespace gmi::math {

/**
 * A fast matrix for 2D transformations, supporting position, rotation, scale, and pivot.
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
    float a = 1;

    /** Shear on the y axis. */
    float b = 0;

    /** Shear on the x axis. */
    float c = 0;

    /** Scale on the y axis. */
    float d = 1;

    /** Translation on the x axis. */
    float x = 0;

    /** Translation on the y axis. */
    float y = 0;

    Color color;

    /**
     * Creates an Affine from a Transform.
     * @param t A transform
     * @return The affine equivalent
     */
    static Affine fromTransform(const Transform& t);

    /**
     * Creates an Affine that applies a translation.
     * @param translation The translation as a Vec2
     * @return The affine
     */
    static Affine translate(Vec2f translation);

    /**
     * Creates an Affine that applies a scaling transformation.
     * @param scale A Vec2f representing the scale
     * @return The affine equivalent
     */
    static Affine scale(Vec2f scale);

    /**
     * Creates an Affine that applies a scaling transformation about a pivot.
     * @param pivot A normalized Vec2f representing the pivot (components 0-1)
     * @param scaleFactor A Vec2f representing the scale
     * @return The affine
     */
    static Affine scaleAbout(Vec2f pivot, Vec2f scaleFactor);
};

inline std::ostream& operator<<(std::ostream& stream, const Affine& m) {
    stream << "Affine" << '\n'
           << "|" << m.a << " " << m.c << " " << m.x << "|" << '\n'
           << "|" << m.b << " " << m.d << " " << m.y << "|" << '\n';
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

    m.color = a.color * b.color;

    return m;
}

inline Affine Affine::fromTransform(const Transform& t) {
    auto [position, rotation, scale, pivot, _] = t;
    Affine m;

    float c = std::cos(rotation);
    float s = std::sin(rotation);

    m.a = c * scale.x;
    m.b = s * scale.x;
    m.c = -s * scale.y;
    m.d = c * scale.y;

    m.x = position.x + pivot.x - ((pivot.x * m.a) + (pivot.y * m.c));
    m.y = position.y + pivot.y - ((pivot.x * m.b) + (pivot.y * m.d));

    m.color = t.color;

    return m;
}

inline Affine Affine::scale(Vec2f scale) {
    Affine m;
    m.a = scale.x;
    m.d = scale.y;
    return m;
}

inline Affine Affine::translate(Vec2f translation) {
    Affine m;
    m.x = translation.x;
    m.y = translation.y;
    return m;
}

inline Affine Affine::scaleAbout(Vec2f pivot, Vec2f scaleFactor) {
    return translate(pivot) * scale(scaleFactor) * translate(-pivot);
}

/**
 * Applies an Affine transformation to a Vec2.
 * @param m The affine to apply
 * @param pos The vector to apply it to
 * @return The transformed vector
 */
inline Vec2f affineApply(const Affine& m, Vec2f pos) {
    auto [x, y] = pos;
    return {
        (m.a * x) + (m.c * y) + m.x,
        (m.b * x) + (m.d * y) + m.y
    };
}

/**
 * Inverts an Affine transformation applied to a Vec2.
 * @param m The affine to reverse
 * @param pos The vector to apply it to
 * @return The transformed vector
 */
inline Vec2f affineApplyInverse(const Affine& m, Vec2f pos) {
    float a = m.a;
    float b = m.b;
    float c = m.c;
    float d = m.d;
    float tx = m.x;
    float ty = m.y;

    float id = 1 / ((a * d) + (c * -b));

    float x = pos.x;
    float y = pos.y;

    return {
        (d * id * x) + (-c * id * y) + (((ty * c) - (tx * d)) * id),
        (a * id * y) + (-b * id * x) + (((-ty * a) + (tx * b)) * id)
    };
}

}
