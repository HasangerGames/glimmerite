#pragma once
#include <cmath>
#include <functional>

#include "math.h"

namespace gmi::math {

using EasingFn = std::function<float(float)>;

/**
 * A collection of functions for easing, based on
 * [this helpful reference](https://easings.net) and others
 */
namespace Easing {
    //
    // Linear
    //

    constexpr float linear(const float t) { return t; }

    //
    // Sine
    //

    constexpr float sineIn(const float t) { return 1 - std::cos(t * PI_HALF); }
    constexpr float sineOut(const float t) { return std::sin(t * PI_HALF); }
    constexpr float sineInOut(const float t) { return (1 - std::cos(PI * t)) / 2; }

    //
    // Circular
    //
    
    inline float circIn(const float t) {
        return 1 - std::sqrt(1 - t*t);
    }
    inline float circOut(const float t) {
        return std::sqrt(1 - (t-1)*(t-1));
    }
    inline float circInOut(const float t) {
        return (t < 0.5)
            ? (1 - std::sqrt(1 - (2*t)*(2*t))) * 0.5f
            : (std::sqrt(1 - (2*(1-t))*(2*(1-t))) + 1) * 0.5f;
    }

    //
    // Elastic
    //
    
    inline float elasticIn(const float t) {
        if (t == 0 || t == 1) return t;
        return -(std::pow(2.f, 10.f*(t - 1))) *
               std::sin(PI * (40*(t - 1) - 3) / 6.f);
    }
    inline float elasticOut(const float t) {
        if (t == 0 || t == 1) return t;
        return std::pow(2.f, -10.f*t) *
               std::sin(PI * (40*t - 3) / 6.f) + 1.f;
    }
    inline float elasticInOut(const float t) {
        if (t == 0 || t == 1) return t;
        if (t < 0.5f) {
            return -(std::pow(2.f, 10.f*(2*t - 1) - 1)) *
                   std::sin(PI * (80*(2*t - 1) - 9) / 18.f);
        }
        return std::pow(2.f, -10.f*(2*t - 1) - 1) *
               std::sin(PI * (80*(2*t - 1) - 9) / 18.f) + 1.f;
    }
    inline float elasticOut2(const float t) {
        return std::pow(2.f, -10.f*t) *
               std::sin(((t - 0.75f/4) * (PI*2)) / 0.75f) + 1;
    }

    //
    // Exponential
    //
    
    inline float expoIn(const float t) {
        return (t <= 0) ? 0.f : std::pow(2.f, -10.f * (1 - t));
    }
    inline float expoOut(const float t) {
        return (t >= 1) ? 1.f : 1.f - std::pow(2.f, -10.f*t);
    }
    inline float expoInOut(const float t) {
        if (t == 0 || t == 1) return t;
        return t < 0.5f
            ? std::pow(2.f, 10*(2*t - 1) - 1)
            : 1 - std::pow(2.f, -10*(2*t - 1) - 1);
    }

    //
    // Polynomial
    //

    constexpr float polyIn(const float t, const int d) {
        return std::pow(t, d);
    }
    constexpr float polyOut(const float t, const int d) {
        return 1 - std::pow(1 - t, d);
    }
    constexpr float polyInOut(const float t, const int d) {
        const float c = std::pow(2.f, d - 1.f);
        return t < 0.5f
            ? c * std::pow(t, d)
            : 1.f - c * std::pow(1.f - t, d);
    }

    inline float quadraticIn(const float t) { return polyIn(t, 2); }
    inline float quadraticOut(const float t) { return polyOut(t, 2); }
    inline float quadraticInOut(const float t) { return polyInOut(t, 2); }

    inline float cubicIn(const float t) { return polyIn(t, 3); }
    inline float cubicOut(const float t) { return polyOut(t, 3); }
    inline float cubicInOut(const float t) { return polyInOut(t, 3); }

    inline float quarticIn(const float t) { return polyIn(t, 4); }
    inline float quarticOut(const float t) { return polyOut(t, 4); }
    inline float quarticInOut(const float t) { return polyInOut(t, 4); }

    inline float quinticIn(const float t) { return polyIn(t, 5); }
    inline float quinticOut(const float t) { return polyOut(t, 5); }
    inline float quinticInOut(const float t) { return polyInOut(t, 5); }

    inline float sexticIn(const float t) { return polyIn(t, 6); }
    inline float sexticOut(const float t) { return polyOut(t, 6); }
    inline float sexticInOut(const float t) { return polyInOut(t, 6); }

    //
    // Back
    //

    inline float backIn(const float t) {
        return (std::sqrt(3.f)*(t - 1) + t) * t*t;
    }
    inline float backOut(const float t) {
        return 1.0f + ((std::sqrt(3.f)+1)*t - 1)*std::pow(t - 1, 2);
    }
    inline float backInOut(const float t) {
        return t < 0.5f
            ? 4.0f * t*t * (3.6f*t - 1.3f)
            : 4 * std::pow(t - 1, 2) * (3.6f*t - 2.3f) + 1.0f;
    }
}

}
