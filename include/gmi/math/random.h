#pragma once

#include <random>

namespace gmi::math {

static std::mt19937& rng() {
    static auto gen = std::mt19937(std::random_device()());
    return gen;
}

inline int random(int min, int max) {
    return std::uniform_int_distribution(min, max)(rng());
}

}
