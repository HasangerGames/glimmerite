#pragma once

#include "Easing.h"

namespace gmi::math {

template <typename T>
struct TweenMember {
    float T::* member;
    float targetValue;
};

template<typename T>
struct Tween {
    T* target;
    uint64_t startTime;
    uint64_t endTime;
    EasingFn ease;
};

}
