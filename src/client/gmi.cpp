#include "gmi/client/gmi.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef __EMSCRIPTEN__

// #include "bimg/decode.h"

#include <fstream>

#include "bx/allocator.h"

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

Buffer readFileSync(const std::string& path, const std::string& errorMessage) {
    auto stream = std::ifstream(path, std::ios::binary);
    if (!stream.good()) {
        throw GmiException(errorMessage + ": File not found: " + path);
    }

    stream.seekg(0, std::ios::end);
    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    auto data = Buffer(size);
    stream.read(data.data(), size);
    return data;
}

// bx::DefaultAllocator g_alloc;

void readImage(const std::string& path, ImageLoadCallback onLoad, const std::string& errorMessage) {
    int width, height;
    // TODO This method can be used when we switch to async I/O
    // stbi_uc* rawData = stbi_load_from_memory(
    //     reinterpret_cast<const stbi_uc*>(data.data()),
    //     data.size(),
    //     &width,
    //     &height,
    //     nullptr,
    //     4
    // );
    stbi_uc* rawData = stbi_load(
        path.c_str(),
        &width,
        &height,
        nullptr,
        4
    );
    if (rawData == nullptr) {
        throw GmiException(errorMessage + ": File not found: " + path);
    }
    // Buffer buf = readFileSync(path, errorMessage);
    // bimg::ImageContainer* image = bimg::imageParse(&g_alloc, buf.data(), buf.size());
    //
    // onLoad({
    //     .width = image->m_width,
    //     .height = image->m_height,
    //     .data = bgfx::makeRef(
    //         image->m_data,
    //         image->m_size,
    //         [](void*, void* userData) { bimg::imageFree(static_cast<bimg::ImageContainer*>(userData)); },
    //         image
    //     ),
    // });
}

}

#else // using emscripten

#include <emscripten.h>
#include <emscripten/html5.h>

#include <cstring>

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

Buffer readFileSync(const std::string& path, const std::string& errorMessage) {
    void* bufferPtr;
    int size;
    int error;
    emscripten_wget_data(path.c_str(), &bufferPtr, &size, &error);

    if (error) {
        std::cout << error << '\n';
        free(bufferPtr);
        throw GmiException(errorMessage + ": File not found: " + path);
    }

    auto data = Buffer(size);
    std::memcpy(data.data(), bufferPtr, size);
    free(bufferPtr);
    return data;
}

struct ImageLoadContext {
    ImageLoadCallback onLoad;
    ErrorCallback onError;
};

extern "C" {
    void readImageOnLoad(void* userData, uint32_t width, uint32_t height, void* buffer, int size) {
        auto* ctx = static_cast<ImageLoadContext*>(userData);
        ctx->onLoad({
            .width = width,
            .height = height,
            .data = bgfx::makeRef(
                buffer,
                size,
                [](void* ptr, void*) { free(ptr); }
            ),
        });
        delete ctx;
    }

    void readImageOnError(void* userData) {
        auto* ctx = static_cast<ImageLoadContext*>(userData);
        ctx->onError();
        delete ctx;
    }
}

void readImage(const std::string& path, ImageLoadCallback onLoad, const std::string& errorMessage) {
    auto* ctx = new ImageLoadContext(
        std::move(onLoad),
        [errorMessage, path] { throw GmiException(errorMessage + ": File not found: " + path); }
    );
    EM_ASM({
        const url = UTF8ToString($0);
        const userData = $1;
        fetch(url)
            .then(response => response.blob())
            .then(blob => createImageBitmap(blob))
            .then(img => {
                const { width, height } = img;
                const canvas = new OffscreenCanvas(width, height);
                const ctx = canvas.getContext('2d');
                ctx.drawImage(img, 0, 0);
                const imageData = ctx.getImageData(0, 0, width, height).data;
                const imageSize = imageData.length;
                const imageDataPtr = _malloc(imageSize);
                HEAPU8.set(imageData, imageDataPtr);
                _readImageOnLoad(userData, width, height, imageDataPtr, imageSize);
            })
            .catch((e) => { console.error(e); _readImageOnError(userData); });
    }, path.c_str(), ctx);
}

// void readImage(const std::string& path, ImageLoadCallback onLoad, const std::string& errorMessage) {
//     auto* ctx = new ImageLoadContext(
//         std::move(onLoad),
//         [errorMessage, path] { throw GmiException(errorMessage + ": File not found: " + path); }
//     );
//     emscripten_async_wget_data(
//         path.c_str(),
//         ctx,
//         [](void* userData, void* buffer, int size) {
//             int width, height;
//             stbi_uc* rawData = stbi_load_from_memory(
//                 reinterpret_cast<const stbi_uc*>(buffer),
//                 size,
//                 &width,
//                 &height,
//                 nullptr,
//                 4
//             );
//             auto* ctx = static_cast<ImageLoadContext*>(userData);
//             ctx->onLoad({
//                 .width = (uint32_t) width,
//                 .height = (uint32_t) height,
//                 .data = bgfx::copy(rawData, width * height * 4),
//                 // .data = bgfx::makeRef(
//                 //     rawData,
//                 //     width * height * 4,
//                 //     [](void* ptr, void*) { stbi_image_free(ptr); }
//                 // ),
//             });
//             delete ctx;
//         },
//         [](void* userData) {
//             auto* ctx = static_cast<ImageLoadContext*>(userData);
//             ctx->onError();
//             delete ctx;
//         }
//     );
// }

}

#endif
