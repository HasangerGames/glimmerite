#pragma once

#include <utility>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>

namespace gmi {

using Buffer = std::vector<char>;

/**
 * Simple timer for profiling code.
 */
class Timer {
public:
    Timer() : Timer("") { }
    explicit Timer(std::string name) :
        m_name(std::move(name)), m_start(std::chrono::steady_clock::now()) { }

    void stop() {
        stop(m_name);
    }

    void stop(const std::string& name, double threshold = 0) {
        if (m_stopped) return;
        m_stopped = true;

        auto elapsed = std::chrono::steady_clock::now() - m_start;
        auto ms = std::chrono::duration<double, std::milli>(elapsed).count();
        if (ms < threshold) return;
        std::cout << std::format("{}: {:.3f} ms\n", name, ms);
    }
private:
    std::string m_name;
    std::chrono::steady_clock::time_point m_start;
    bool m_stopped = false;
};

}
