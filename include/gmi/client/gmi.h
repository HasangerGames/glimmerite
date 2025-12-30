#pragma once

#include <chrono>
#include <functional>

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

using Buffer = std::vector<char>;

using LoadCallback = std::function<void(const Buffer&)>;
using ErrorCallback = std::function<void()>;

void readFile(const std::string& path, LoadCallback onLoad, ErrorCallback onError);

Buffer readFileSync(const std::string& path);

}
