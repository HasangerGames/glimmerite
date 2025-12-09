#pragma once

#include <numbers>

namespace gmi::math {

constexpr float PI = std::numbers::pi_v<float>;
constexpr float TAU = PI * 2;
constexpr float PI_HALF = PI / 2;
constexpr float EPSILON = 0.001;

/**
 * Checks if two numbers are nearly equal.
 * @param a The first number
 * @param b The second number
 * @param epsilon The error tolerance, defaults to 0.001
 * @return Whether the two numbers are nearly equal
 */
inline bool nearlyEqual(float a, float b, float epsilon = EPSILON) {
    return std::abs(a - b) < epsilon * std::max(std::abs(a), std::abs(b));
}

/**
 * Checks if a number is nearly zero.
 * @param n The number
 * @param epsilon The error tolerance, defaults to 0.001
 * @return Whether the number is nearly zero
 */
inline bool nearlyZero(float n, float epsilon = EPSILON) {
    float an = std::abs(n);
    return an < epsilon * an;
}

/**
 * Interpolates between two values.
 * @param start The start value
 * @param end The end value
 * @param interpFactor The interpolation factor
 * @return The number corresponding to the linear interpolation between `start` and `end` at factor `interpFactor`
 */
inline float lerp(float start, float end, float interpFactor) {
    return start * (1 - interpFactor) + end * interpFactor;
}

/**
 * Clamps a number to specified bounds.
 * @param value The number to clamp
 * @param min The minimum value the number can be
 * @param max The maximum value the number can be
 * @returns The number, clamped to the interval `[min, max]`
 */
inline float clamp(float value, float min, float max) {
    return value < max ? value > min ? value : min : max;
}

/**
 * Like std::fmod, but negative numbers cycle back around:
 * `fmodAbs(-1, 4)` gives `3` and `-1 % 4` gives `-1`
 * @param a The dividend
 * @param n The divisor
 * @return The remainder of the division
 */
inline float fmodAbs(float a, float n) {
    return std::fmod(std::fmod(a, n) + n, n);
}

/**
 * Converts degrees to radians.
 * @param degrees An angle in degrees
 * @return The angle in radians
 */
inline float degreesToRadians(float degrees) {
    return degrees * PI / 180.0f;
}

/**
 * Converts radians to degrees.
 * @param radians An angle in radians
 * @return The angle in degrees
 */
inline float radiansToDegrees(float radians) {
    return radians * 180.0f / PI;
}

/**
 * Normalizes an angle to a value between `-π` and `π`
 * @param radians The angle, in radians
 * @return The normalized angle
 */
inline float normalizeAngle(float radians) {
    return fmodAbs(radians - PI, PI * 2) - PI;
}

}
