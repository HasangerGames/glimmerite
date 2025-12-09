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
    const Application& parentApp,
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

bgfx::RendererType::Enum Renderer::getType() { // TODO Move to Application::getRendererType()
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
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, colorToNumber(color));
}

void Renderer::queueDrawable(const Drawable& drawable) {
    m_queue.push_back(drawable);
}

void Renderer::renderFrame() {
    std::vector<math::Vertex> batchVertices;
    bgfx::TextureHandle* batchTexture{nullptr};
    for (auto& [currentVertices, currentTexture] : m_queue) {
        if (currentTexture != batchTexture) {
            if (!batchVertices.empty()) {
                submitBatch(batchVertices, *batchTexture);
                batchVertices.clear();
            }
            batchTexture = currentTexture;
        }

        batchVertices.insert(
            batchVertices.end(),
            std::make_move_iterator(currentVertices.begin()),
            std::make_move_iterator(currentVertices.end())
        );
    }
    submitBatch(batchVertices, *batchTexture);
    m_queue.clear();

    bgfx::frame();
}

constexpr size_t VERT_SIZE = sizeof(math::Vertex);

void Renderer::submitBatch(const std::vector<math::Vertex>& vertices, bgfx::TextureHandle texture) const {
    size_t numVertices = vertices.size();
    size_t numQuads = numVertices / 4;

    // Create vertex buffer
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::allocTransientVertexBuffer(&vertexBuffer, numVertices, m_vertexLayout);
    std::memcpy(vertexBuffer.data, vertices.data(), numVertices * VERT_SIZE);
    bgfx::setVertexBuffer(0, &vertexBuffer);

    // Create index buffer
    bgfx::TransientIndexBuffer indexBuffer;
    bgfx::allocTransientIndexBuffer(&indexBuffer, numQuads * 6);
    auto indices = std::bit_cast<uint16_t*>(indexBuffer.data);
    uint16_t idx = -1;
    for (uint16_t i = 0; i < numQuads; i++) {
        uint16_t base = i * 4;
        indices[++idx] = base + 0;
        indices[++idx] = base + 1;
        indices[++idx] = base + 2;
        indices[++idx] = base + 0;
        indices[++idx] = base + 2;
        indices[++idx] = base + 3;
    }
    bgfx::setIndexBuffer(&indexBuffer);

    bgfx::setTexture(0, m_sampler, texture);

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);

    bgfx::submit(0, m_spriteProgram);
}

void Renderer::shutdown() const {
    bgfx::destroy(m_spriteProgram);
    bgfx::destroy(m_sampler);
    bgfx::shutdown();
}

}
