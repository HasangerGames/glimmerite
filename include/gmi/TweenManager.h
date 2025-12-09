#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "math/Easing.h"

namespace gmi {

struct TweenVar {
    float* var;
    float endValue;
    float startValue;
};

struct TweenOptions {
    std::vector<TweenVar> values;
    uint64_t duration;
    math::EasingFn ease = math::Easing::linear;
    bool yoyo = false;
    bool infinite = false;
    std::function<void()> onUpdate = nullptr;
    std::function<void()> onComplete = nullptr;
};

struct Tween {
    TweenOptions opts;
    uint64_t startTime;
    uint64_t endTime;
};

class TweenManager {
    std::vector<Tween> m_tweens;
public:
    void add(const TweenOptions& opts);

    void update();
};

}
