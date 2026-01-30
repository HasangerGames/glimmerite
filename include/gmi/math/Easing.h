#pragma once
#include "gmi/math/math.h"
#include <cmath>

namespace gmi::math {

using EasingFn = float (*)(float);

/**
 * A collection of functions for easing, based on
 * [this helpful reference](https://easings.net) and others
 */
namespace Easing {
//
// Linear
//

constexpr float linear(float t) { return t; }

//
// Sine
//

constexpr float sineIn(float t) { return 1 - std::cos(t * PI_HALF); }
constexpr float sineOut(float t) { return std::sin(t * PI_HALF); }
constexpr float sineInOut(float t) { return (1 - std::cos(PI * t)) / 2; }

//
// Circular
//

constexpr float circIn(float t) {
    return 1 - std::sqrt(1 - t * t);
}
constexpr float circOut(float t) {
    return std::sqrt(1 - (t - 1) * (t - 1));
}
constexpr float circInOut(float t) {
    return (t < 0.5)
        ? (1 - std::sqrt(1 - (2 * t) * (2 * t))) * 0.5f
        : (std::sqrt(1 - (2 * (1 - t)) * (2 * (1 - t))) + 1) * 0.5f;
}

//
// Elastic
//

constexpr float elasticIn(float t) {
    if (t == 0 || t == 1)
        return t;
    return -(std::pow(2.f, 10.f * (t - 1))) * std::sin(PI * (40 * (t - 1) - 3) / 6.f);
}
constexpr float elasticOut(float t) {
    if (t == 0 || t == 1)
        return t;
    return std::pow(2.f, -10.f * t) * std::sin(PI * (40 * t - 3) / 6.f) + 1.f;
}
constexpr float elasticInOut(float t) {
    if (t == 0 || t == 1)
        return t;
    if (t < 0.5f) {
        return -(std::pow(2.f, 10.f * (2 * t - 1) - 1)) * std::sin(PI * (80 * (2 * t - 1) - 9) / 18.f);
    }
    return std::pow(2.f, -10.f * (2 * t - 1) - 1) * std::sin(PI * (80 * (2 * t - 1) - 9) / 18.f) + 1.f;
}
constexpr float elasticOut2(float t) {
    return std::pow(2.f, -10.f * t) * std::sin(((t - 0.75f / 4) * (PI * 2)) / 0.75f) + 1;
}

//
// Exponential
//

constexpr float expoIn(float t) {
    return (t <= 0) ? 0.f : std::pow(2.f, -10.f * (1 - t));
}
constexpr float expoOut(float t) {
    return (t >= 1) ? 1.f : 1.f - std::pow(2.f, -10.f * t);
}
constexpr float expoInOut(float t) {
    if (t == 0 || t == 1)
        return t;
    return t < 0.5f
        ? std::pow(2.f, 10 * (2 * t - 1) - 1)
        : 1 - std::pow(2.f, -10 * (2 * t - 1) - 1);
}

//
// Polynomial
//

constexpr float polyIn(float t, int d) {
    return std::pow(t, d);
}
constexpr float polyOut(float t, int d) {
    return 1 - std::pow(1 - t, d);
}
constexpr float polyInOut(float t, int d) {
    float c = std::pow(2.f, d - 1.f);
    return t < 0.5f
        ? c * std::pow(t, d)
        : 1.f - c * std::pow(1.f - t, d);
}

constexpr float quadraticIn(float t) { return polyIn(t, 2); }
constexpr float quadraticOut(float t) { return polyOut(t, 2); }
constexpr float quadraticInOut(float t) { return polyInOut(t, 2); }

constexpr float cubicIn(float t) { return polyIn(t, 3); }
constexpr float cubicOut(float t) { return polyOut(t, 3); }
constexpr float cubicInOut(float t) { return polyInOut(t, 3); }

constexpr float quarticIn(float t) { return polyIn(t, 4); }
constexpr float quarticOut(float t) { return polyOut(t, 4); }
constexpr float quarticInOut(float t) { return polyInOut(t, 4); }

constexpr float quinticIn(float t) { return polyIn(t, 5); }
constexpr float quinticOut(float t) { return polyOut(t, 5); }
constexpr float quinticInOut(float t) { return polyInOut(t, 5); }

constexpr float sexticIn(float t) { return polyIn(t, 6); }
constexpr float sexticOut(float t) { return polyOut(t, 6); }
constexpr float sexticInOut(float t) { return polyInOut(t, 6); }

//
// Back
//

constexpr float backIn(float t) {
    return (std::numbers::sqrt3_v<float> * (t - 1) + t) * t * t;
}
constexpr float backOut(float t) {
    return 1.0f + ((std::numbers::sqrt3_v<float> + 1) * t - 1) * std::pow(t - 1, 2);
}
constexpr float backInOut(float t) {
    return t < 0.5f
        ? 4.0f * t * t * (3.6f * t - 1.3f)
        : 4 * std::pow(t - 1, 2) * (3.6f * t - 2.3f) + 1.0f;
}
}

}
