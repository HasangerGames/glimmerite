#pragma once

#include <chrono>

namespace gmi {

struct GmiException : std::runtime_error {
    using std::runtime_error::runtime_error;
};

inline uint64_t nowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

}
