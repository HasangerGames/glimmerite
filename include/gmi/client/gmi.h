#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>

#include "bgfx/bgfx.h"

namespace gmi {

/** Generic exception class used by Glimmerite. */
struct GmiException final : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/** @return The number of milliseconds since the epoch */
inline uint64_t nowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

inline std::string nameFromPath(const std::string& path) {
    return std::filesystem::path(path).stem().string();
}

/**
 * Prints the time elapsed since the object's creation when it is destroyed.
 * Can be used to profile functions, or smaller blocks of code by enclosing in braces.
 */
class Timer {
public:
    explicit Timer(const std::string& name) :
        m_name(name), m_start(std::chrono::steady_clock::now()) { }
    ~Timer() {
        auto elapsed = std::chrono::steady_clock::now() - m_start;
        auto ms = std::chrono::duration<double, std::milli>(elapsed);
        std::cout << std::format("{}: {:.3f} ms\n", m_name, ms.count());
    }
    void stop() const {
        delete this;
    }
private:
    std::string m_name;
    std::chrono::steady_clock::time_point m_start;
};

using Buffer = std::vector<char>;

using LoadCallback = std::function<void(const Buffer&)>;
using ErrorCallback = std::function<void()>;

void readFile(const std::string& path, LoadCallback onLoad, ErrorCallback onError);

Buffer readFileSync(const std::string& path, const std::string& errorMessage);

struct Image {
    uint32_t width, height;
    const bgfx::Memory* data;
};

using ImageLoadCallback = std::function<void(const Image&)>;

void readImage(const std::string& path, ImageLoadCallback onLoad, const std::string& errorMessage);

}
