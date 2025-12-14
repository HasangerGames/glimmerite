#include "gmi/Renderer.h"

#include <cstring>

#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bx/math.h"
#include "gmi/Application.h"
#include "gmi/gmi.h"
#include "shaders/shaders.h"

namespace gmi {

void Renderer::init(
    Application& parentApp,
    uint32_t width,
    uint32_t height,
    bool vsync,
    bgfx::RendererType::Enum rendererType
) {
    if (m_initialized) {
        throw GmiException("Renderer has already been initialized");
    }

    m_parentApp = &parentApp;
    m_width = width;
    m_height = height;

    bgfx::Init init;
    init.type = rendererType;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
    const SDL_PropertiesID props = SDL_GetWindowProperties(parentApp.getWindow());
#if defined(SDL_PLATFORM_WIN32)
    init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#elif defined(SDL_PLATFORM_MACOS)
    init.platformData.nwh = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
#elif defined(SDL_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        init.platformData.nwh = reinterpret_cast<void*>(SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0)); // NOLINT(performance-no-int-to-ptr)
        init.platformData.ndt = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        init.platformData.type = bgfx::NativeWindowHandleType::Wayland;
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

    static constexpr bx::Vec3 eye{0.0f, 0.0f, -1.0f};
    static constexpr bx::Vec3 at{0.0f, 0.0f, 0.0f};
    bx::mtxLookAt(m_viewMatrix, eye, at);
    resize(width, height);

    bgfx::RendererType::Enum actualRenderer = bgfx::getRendererType();
    m_spriteProgram = bgfx::createProgram(
        bgfx::createEmbeddedShader(&internal::VS_SPRITE, actualRenderer, "vs_sprite"),
        bgfx::createEmbeddedShader(&internal::FS_SPRITE, actualRenderer, "fs_sprite"),
        true
    );
    m_colorProgram = bgfx::createProgram(
        bgfx::createEmbeddedShader(&internal::VS_COLOR, actualRenderer, "vs_color"),
        bgfx::createEmbeddedShader(&internal::FS_COLOR, actualRenderer, "fs_color"),
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

bgfx::RendererType::Enum Renderer::getType() {
    return bgfx::getRendererType();
}

void Renderer::setVsync(bool vsync) {
    m_vsync = vsync;
    reset();
}

void Renderer::resize(uint32_t width, uint32_t height) {
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

    reset();
}

void Renderer::reset() const {
    bgfx::reset(m_width, m_height, m_vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
}

void Renderer::setClearColor(const Color& color) {
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, color.rgbaHex());
}

void Renderer::queueDrawable(const Drawable& drawable) {
    const auto& [vertices, indices, texture] = drawable;

    if (texture.idx != m_batchTexture.idx) {
        submitBatch();
        m_batchTexture = texture;
    }

    size_t numVertices = m_batchVertices.size();
    for (uint16_t index : indices) {
        m_batchIndices.push_back(numVertices + index);
    }

    m_batchVertices.insert(m_batchVertices.end(), vertices.begin(), vertices.end());
}

void Renderer::submitBatch() {
    if (m_batchVertices.empty())
        return;

    static constexpr size_t VERT_SIZE = sizeof(Vertex);
    static constexpr size_t IND_SIZE = sizeof(uint16_t);

    bgfx::TransientVertexBuffer vertexBuffer{};
    size_t numVertices = m_batchVertices.size();
    bgfx::allocTransientVertexBuffer(&vertexBuffer, numVertices, m_vertexLayout);
    std::memcpy(vertexBuffer.data, m_batchVertices.data(), numVertices * VERT_SIZE);
    bgfx::setVertexBuffer(0, &vertexBuffer);

    bgfx::TransientIndexBuffer indexBuffer{};
    size_t numIndices = m_batchIndices.size();
    bgfx::allocTransientIndexBuffer(&indexBuffer, numIndices);
    std::memcpy(indexBuffer.data, m_batchIndices.data(), numIndices * IND_SIZE);
    bgfx::setIndexBuffer(&indexBuffer);

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);

    if (bgfx::isValid(m_batchTexture)) {
        bgfx::setTexture(0, m_sampler, m_batchTexture);
        bgfx::submit(0, m_spriteProgram);
    } else {
        bgfx::submit(0, m_colorProgram);
    }

    m_batchVertices.clear();
    m_batchIndices.clear();
    m_batchTexture = BGFX_INVALID_HANDLE;
}

void Renderer::render(Container& container) {
    container.render(*this);
    submitBatch();
    bgfx::frame();
}

void Renderer::shutdown() const {
    bgfx::destroy(m_spriteProgram);
    bgfx::destroy(m_colorProgram);
    bgfx::destroy(m_sampler);
    bgfx::shutdown();
}

}
