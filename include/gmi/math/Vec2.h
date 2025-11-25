#pragma once
#include <cmath>

#include "math.h"

namespace gmi::math {

struct Vec2 { float x = 0, y = 0; };

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
 * @param a The vector to measure
 * @return The length of the vector
 */
inline float length(const Vec2& a) {
    return std::sqrt(a.x * a.x + a.y * a.y);
}

/**
 * Calculates the length of a vector, using std::hypot to avoid overflow/underflow.
 * This method is significantly slower than @ref length, so use that if possible.
 * @param a The vector to measure
 * @return The length of the vector
 */
inline float lengthSafe(const Vec2& a) {
    return std::hypot(a.x, a.y);
}

/**
 * Calculates the squared length of a vector.
 * @param a The vector to measure
 * @return The length of the vector
 */
inline float lengthSquared(const Vec2& a) {
    return a.x * a.x + a.y * a.y;
}

/**
 * Creates a new vector parallel to the original, but whose length is 1.
 * @param a The vector to normalize
 * @returns A vector whose length is 1 and is parallel to the original vector
 */
inline Vec2 normalize(const Vec2& a) {
    const float len = length(a);
    return nearlyEqual(len, 0)
        ? Vec2{ a.x / len, a.y / len }
        : Vec2{ a.x, a.y };
}

/**
 * Returns the additive inverse of this vector; in other words, a vector that, when added to this one, gives the zero vector
 * @param a The vector to invert
 * @returns A vector whose components are -1 multiplied by the corresponding component in the original vector
 */
inline Vec2 invert(const Vec2& a) {
    return { -a.x, -a.y };
}

//
// Operations involving two vectors
//



}
