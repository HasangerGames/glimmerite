#pragma once
#include <vector>

#include "gmi/client/Color.h"
#include "gmi/client/Drawable.h"
#include "bgfx/bgfx.h"

namespace gmi {

class Application;
class Container;

/**
 * The Renderer is an API which handles communication between the Application and bgfx.
 */
class Renderer {
    bool m_initialized = false;
    Application* m_parentApp = nullptr;
    uint32_t m_width = 0, m_height = 0;
    bool m_vsync = true;
    float m_viewMatrix[16] = {};
    float m_projMatrix[16] = {};
    bgfx::ProgramHandle m_spriteProgram = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle m_colorProgram = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_sampler = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout m_vertexLayout;

    void reset() const;

    std::vector<Vertex> m_batchVertices;
    std::vector<uint16_t> m_batchIndices;
    bgfx::TextureHandle m_batchTexture = BGFX_INVALID_HANDLE;
    void submitBatch();
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    void init(Application& parentApp, uint32_t width, uint32_t height, bool vsync, bgfx::RendererType::Enum rendererType);

    /** @return The type of renderer being used. */
    [[nodiscard]] static bgfx::RendererType::Enum getType();

    /** Controls VSync. See @ref Application for more info. */
    void setVsync(bool vsync);

    void resize(uint32_t width, uint32_t height);

    static void setClearColor(const Color& color);

    void queueDrawable(const Drawable& drawable);

    void render(Container& container);

    void shutdown() const;
};

}
