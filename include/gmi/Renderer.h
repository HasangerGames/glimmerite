#pragma once
#include <vector>

#include "Drawable.h"
#include "Color.h"
#include "bgfx/bgfx.h"

namespace gmi {

class Application;
class Container;

/**
 * The Renderer is an API which handles communication between the Application and bgfx.
 */
class Renderer {
    const Application *m_parentApp;
    uint32_t m_width, m_height;
    bool m_vsync;
    float m_viewMatrix[16];
    float m_projMatrix[16];
    bgfx::ProgramHandle m_spriteProgram;
    bgfx::UniformHandle m_sampler;
    bgfx::VertexLayout m_vertexLayout;
    bool m_initialized = false;

    void reset() const;

    std::vector<math::Vertex> m_batchVertices;
    bgfx::TextureHandle* m_batchTexture = nullptr;
    void submitBatch();
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    void init(const Application& parentApp, uint32_t width, uint32_t height, bool vsync, bgfx::RendererType::Enum rendererType);

    /** @return The type of renderer being used. */
    [[nodiscard]] bgfx::RendererType::Enum getType();

    /** Controls VSync. See @ref Application for more info. */
    void setVsync(bool vsync);

    void resize(uint32_t width, uint32_t height);

    static void setClearColor(const Color& color);

    void queueDrawable(const Drawable& drawable);

    void render(Container& container);

    void shutdown() const;
};

}
