#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "gmi/math/Easing.h"

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
public:
    uint16_t add(const TweenOptions& opts);
    bool kill(uint16_t id);

    void update();
private:
    std::unordered_map<uint16_t, Tween> m_tweens;
    uint16_t m_nextId = 0;
};

}
