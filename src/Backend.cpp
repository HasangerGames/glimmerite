#include "gmi/Backend.h"

#include <cstring>

#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bx/math.h"
#include "gmi/Application.h"
#include "gmi/gmi.h"
#include "gmi/Sprite.h"
#include "shaders/shaders.h"

namespace gmi {

void Backend::init(const Application &parentApp, const uint32_t width, const uint32_t height, const bgfx::RendererType::Enum rendererType) {
    if (m_initialized) {
        throw GmiException("Backend has already been initialized");
    }

    m_parentApp = &parentApp;
    m_width = width;
    m_height = height;

    bgfx::Init init;
    init.type = rendererType;
    init.resolution.width = width;
    init.resolution.height = height;
    const SDL_PropertiesID props = SDL_GetWindowProperties(parentApp.getWindow());
#if defined(SDL_PLATFORM_WIN32)
    init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#elif defined(SDL_PLATFORM_MACOS)
    init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
#elif defined(SDL_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        init.platformData.nwh = reinterpret_cast<void*>(SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
        init.platformData.ndt = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        init.platformData.ndt = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    }
#elif defined(SDL_PLATFORM_IOS)
    init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, nullptr);
#elif defined(SDL_PLATFORM_ANDROID)
    init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_ANDROID_SURFACE_POINTER, nullptr);
    init.platformData.ndt = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_ANDROID_DISPLAY_POINTER, nullptr);
#elif defined(EMSCRIPTEN)
    init.platformData.nwh = reinterpret_cast<void*>("#canvas");
#endif
    bgfx::init(init);

    constexpr bx::Vec3 eye{ 0.0f, 0.0f, -1.0f };
    constexpr bx::Vec3 at { 0.0f, 0.0f, 0.0f };
    bx::mtxLookAt(m_viewMatrix, eye, at);
    resize(width, height);

    m_spriteProgram = bgfx::createProgram(
        bgfx::createEmbeddedShader(&internal::VS_SPRITE, bgfx::getRendererType(), "vs_sprite"),
        bgfx::createEmbeddedShader(&internal::FS_SPRITE, bgfx::getRendererType(), "fs_sprite"),
        true
    );

    m_sampler = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

    m_vertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    m_initialized = true;
}


void Backend::setVsync(const bool vsync) const {
    bgfx::reset(m_width, m_height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
}

void Backend::resize(const uint32_t width, const uint32_t height) {
    m_width = width;
    m_height = height;

    bx::mtxOrtho(
        m_projMatrix,
        0,
        static_cast<float>(width),
        static_cast<float>(height),
        0,
        0,
        1,
        0,
        false
    );

    bgfx::setViewTransform(0, m_viewMatrix, m_projMatrix);
    bgfx::setViewRect(0, 0, 0, width, height);
}

Texture& Backend::loadTexture(const std::string& filePath) {
    m_textures.emplace_back(filePath);
    return m_textures.back();
}

void Backend::setClearColor(const Color color) {
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, colorToNumber(color));
}

void Backend::queueDrawable(const Drawable& drawable) {
    m_queue.push_back(drawable);
}

void Backend::renderFrame() {
    for (auto& [vertices, textureRef] : m_queue) {
        const size_t numVertices = vertices.size();
        bgfx::TransientVertexBuffer vertexBuffer;
        bgfx::allocTransientVertexBuffer(&vertexBuffer, numVertices, m_vertexLayout);
        std::memcpy(vertexBuffer.data, vertices.data(), numVertices * sizeof(math::Vertex));
        bgfx::setVertexBuffer(0, &vertexBuffer);

        const bgfx::TextureHandle texture = textureRef->getRawTexture();
        bgfx::setTexture(0, m_sampler, texture);

        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);

        bgfx::submit(0, m_spriteProgram);
    }
    m_queue.clear();

    bgfx::frame();
}

void Backend::shutdown() {
    for (Texture& texture : m_textures) {
        bgfx::destroy(texture.getRawTexture());
    }
    bgfx::destroy(m_spriteProgram);
    bgfx::destroy(m_sampler);
    bgfx::shutdown();
}

}
