#pragma once
#include <cmath>

#include "math.h"

namespace gmi::math {
/** Represents a two-dimensional vector. */
struct Vec2 {
    /** The x-component of the vector. */
    float x{0};
    /** The y-component of the vector. */
    float y{0};

    /** Creates an empty Vec2. */
    Vec2() = default;

    /**
     * Creates a Vec2 with float components.
     * @param x The x-component
     * @param y The y-component
     */
    Vec2(const float x, const float y) : x(x), y(y) {}

    /**
     * Creates a Vec2 with int components.
     * @param x The x-component
     * @param y The y-component
     */
    Vec2(const int x, const int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
};

inline std::ostream& operator<<(std::ostream& stream, const Vec2& a) {
    stream << "{" << a.x << ", " << a.y << "}";
    return stream;
}

//
// Arithmetic operations
//

inline bool operator==(const Vec2& a, const Vec2& b) {
    return nearlyEqual(a.x, b.x) && nearlyEqual(a.y, b.y);
}

inline bool operator!=(const Vec2& a, const Vec2& b) {
    return !(a == b);
}

inline Vec2 operator+(const Vec2& a, const Vec2& b) {
    return { a.x + b.x, a.y + b.y };
}

inline Vec2 operator-(const Vec2& a, const Vec2& b) {
    return { a.x - b.x, a.y - b.y };
}

inline Vec2 operator*(const Vec2& a, const Vec2& b) {
    return { a.x * b.x, a.y * b.y };
}

inline Vec2 operator/(const Vec2& a, const Vec2& b) {
    return { a.x / b.x, a.y / b.y };
}

inline Vec2& operator+=(Vec2& a, const Vec2& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline Vec2& operator-=(Vec2& a, const Vec2& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

inline Vec2& operator*=(Vec2& a, const Vec2& b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
}

inline Vec2& operator/=(Vec2& a, const Vec2& b) {
    a.x /= b.x;
    a.y /= b.y;
    return a;
}

inline Vec2 operator+(const Vec2& a, const float n) {
    return { a.x + n, a.y + n };
}
inline Vec2 operator+(const float n, const Vec2& a) {
    return { a.x + n, a.y + n };
}

inline Vec2 operator-(const Vec2& a, const float n) {
    return { a.x - n, a.y - n };
}
inline Vec2 operator-(const float n, const Vec2& a) {
    return { a.x - n, a.y - n };
}

inline Vec2 operator*(const Vec2& a, const float n) {
    return { a.x * n, a.y * n };
}
inline Vec2 operator*(const float n, const Vec2& a) {
    return { a.x * n, a.y * n };
}

inline Vec2 operator/(const Vec2& a, const float n) {
    return { a.x / n, a.y / n };
}
inline Vec2 operator/(const float n, const Vec2& a) {
    return { a.x / n, a.y / n };
}

inline Vec2& operator+=(Vec2& a, const float n) {
    a.x += n;
    a.y += n;
    return a;
}

inline Vec2& operator-=(Vec2& a, const float n) {
    a.x -= n;
    a.y -= n;
    return a;
}

inline Vec2& operator*=(Vec2& a, const float n) {
    a.x *= n;
    a.y *= n;
    return a;
}

inline Vec2& operator/=(Vec2& a, const float n) {
    a.x /= n;
    a.y /= n;
    return a;
}

//
// Operations involving a single vector
//

/**
 * Calculates the length of a vector.
 * @param v The vector to measure
 * @return The length of the vector
 */
inline float length(const Vec2& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

/**
 * Calculates the length of a vector, using std::hypot to avoid overflow/underflow.
 * This method is significantly slower than length(), so use that if possible.
 * @param v The vector to measure
 * @return The length of the vector
 */
inline float lengthSafe(const Vec2& v) {
    return std::hypot(v.x, v.y);
}

/**
 * Calculates the squared length of a vector.
 * @param v The vector to measure
 * @return The length of the vector
 */
inline float lengthSquared(const Vec2& v) {
    return v.x * v.x + v.y * v.y;
}

/**
 * Creates a new vector parallel to the original, but whose length is 1.
 * @param v The vector to normalize
 * @returns A vector whose length is 1 and is parallel to the original vector
 */
inline Vec2 normalize(const Vec2& v) {
    const float len = length(v);
    return nearlyEqual(len, 0)
        ? Vec2{ v.x / len, v.y / len }
        : Vec2{ v.x, v.y };
}

/**
 * Returns the additive inverse of this vector; in other words, a vector that, when added to this one, gives the zero vector.
 * @param v The vector to invert
 * @returns A vector whose components are -1 multiplied by the corresponding component in the original vector
 */
inline Vec2 invert(const Vec2& v) {
    return { -v.x, -v.y };
}

/**
 * Rotates a vector by a given angle
 * @param v The vector to be rotated
 * @param angle The angle in radians to rotate the vector by
 * @returns A new vector resulting from the rotation of the input vector by the given angle
 */
inline Vec2 rotate(const Vec2& v, const float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    return { v.x * c - v.y * s, v.x * s + v.y * c };
}

//
// Operations involving two vectors
//

/**
 * Interpolate between two vectors.
 * @param start The start vector
 * @param end The end vector
 * @param interpFactor The interpolation factor, ranging from 0 to 1
 * @return The vector corresponding to the linear interpolation between `start` and `end` at factor `interpFactor`
 */
inline Vec2 lerp(const Vec2& start, const Vec2& end, const float interpFactor) {
    return start * (1 - interpFactor) + end * interpFactor;
}

/**
 * Draws a line between two points and returns that line's angle with respect to the horizontal axis.
 * @param a The first point, used as the tail of the vector
 * @param b The second point, used as the head of the vector
 * @return The angle, in radians, of the line going from `a` to `b`
 */
inline float angleBetween(const Vec2& a, const Vec2& b) {
    return std::atan2(a.y - b.y, a.x - b.x);
}

}
