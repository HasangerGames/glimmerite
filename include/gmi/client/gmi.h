#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>

#include "bgfx/bgfx.h"
#include "gmi/util/util.h"

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
