#pragma once

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
 * Interpolates between two values.
 * @param start The start value
 * @param end The end value
 * @param interpFactor The interpolation factor
 * @return The number corresponding to the linear interpolation between `start` and `end` at factor `interpFactor`
 */
inline float lerp(const float start, const float end, const float interpFactor) {
    return start * (1 - interpFactor) + end * interpFactor;
}

/**
 * Like std::fmod, but negative numbers cycle back around:
 * `fmodAbs(-1, 4)` gives `3` and `-1 % 4` gives `-1`
 * @param a The dividend
 * @param n The divisor
 * @return The remainder of the division
 */
inline float fmodAbs(const float a, const float n) {
    return std::fmod(std::fmod(a, n) + n, n);
}

/**
 * Converts degrees to radians.
 * @param degrees An angle in degrees
 * @return The angle in radians
 */
inline float degreesToRadians(const float degrees) {
    return degrees * std::numbers::pi_v<float> / 180.0f;
}

/**
 * Converts radians to degrees.
 * @param radians An angle in radians
 * @return The angle in degrees
 */
inline float radiansToDegrees(const float radians) {
    return radians * 180.0f / std::numbers::pi_v<float>;
}

/**
 * Normalizes an angle to a value between `-π` and `π`
 * @param radians The angle, in radians
 * @return The normalized angle
 */
inline float normalizeAngle(const float radians) {
    constexpr float pi = std::numbers::pi_v<float>;
    return fmodAbs(radians - pi, pi * 2) - pi;
}

}
