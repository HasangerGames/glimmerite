#include "gmi/client/gmi.h"

#include <cstring>

#ifndef __EMSCRIPTEN__
#include <fstream>
#else
#include <emscripten.h>
#include <emscripten/html5.h>
#include <iostream>
#endif

namespace gmi {

void loadFile(const std::string& path, LoadCallback onLoad, ErrorCallback onError) {
#ifndef __EMSCRIPTEN__
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
#else
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
#endif
}

}
