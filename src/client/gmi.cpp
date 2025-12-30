#include "gmi/client/gmi.h"

#include <cstring>

#ifndef __EMSCRIPTEN__

#include <fstream>

namespace gmi {

void readFile(const std::string& path, LoadCallback onLoad, ErrorCallback onError) {
    // TODO Make this actually async
    auto stream = std::ifstream(path, std::ios::binary);
    if (!stream.good()) {
        onError();
    }

    stream.seekg(0, std::ios::end);
    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    auto data = Buffer(size);
    stream.read(data.data(), size);
    onLoad(data);
}

Buffer readFileSync(const std::string& path) {
    auto stream = std::ifstream(path, std::ios::binary);
    if (!stream.good()) {
        throw std::runtime_error("File not found: " + path);
    }

    stream.seekg(0, std::ios::end);
    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    auto data = Buffer(size);
    stream.read(data.data(), size);
    return data;
}

}

#else // using emscripten

#include <emscripten.h>
#include <emscripten/html5.h>

namespace gmi {

void readFile(const std::string& path, LoadCallback onLoad, ErrorCallback onError) {
    struct LoadContext {
        LoadCallback onLoad;
        ErrorCallback onError;
    };

    auto* ctx = new LoadContext(
        std::move(onLoad),
        std::move(onError)
    );
    emscripten_async_wget_data(
        path.c_str(),
        ctx,
        [](void* userData, void* buffer, int size) {
            auto* ctx = static_cast<LoadContext*>(userData);
            auto data = Buffer(size);
            std::memcpy(data.data(), buffer, size);
            ctx->onLoad(data);
            delete ctx;
        },
        [](void* userData) {
            auto* ctx = static_cast<LoadContext*>(userData);
            ctx->onError();
            delete ctx;
        }
    );
}

Buffer readFileSync(const std::string& path) {
    void* bufferPtr;
    int size;
    int error;
    emscripten_wget_data(path.c_str(), &bufferPtr, &size, &error);

    if (error) {
        delete bufferPtr;
        throw std::runtime_error("File not found: " + path);
    }

    auto data = Buffer(size);
    std::memcpy(data.data(), bufferPtr, size);
    delete bufferPtr;
    return data;
}

}

#endif
