#pragma once
#include <cmath>

namespace gmi::math {

constexpr float EPSILON = 0.001;

/**
 * Checks if two numbers are nearly equal.
 * @param a The first number
 * @param b The second number
 * @param epsilon The error tolerance, defaults to 0.001
 * @return Whether the two numbers are nearly equal
 */
inline bool nearlyEqual(const float a, const float b, const float epsilon = EPSILON) {
    return std::abs(a - b) < epsilon * std::max(std::abs(a), std::abs(b));
}

/**
 * Interpolates between two values
 * @param start The start value
 * @param end The end value
 * @param interpFactor The interpolation factor
 * @return A number corresponding to the linear interpolation between `a` and `b` at factor `interpFactor`
 */
inline float lerp(const float start, const float end, const float interpFactor) {
    return start * (1 - interpFactor) + end * interpFactor;
}

}
