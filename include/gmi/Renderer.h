#pragma once
#include <chrono>
#include <deque>
#include <vector>

#include "Drawable.h"
#include "Color.h"
#include "bgfx/bgfx.h"

namespace gmi {

class Application;

/**
 * The Renderer is an API which handles communication between the Application and bgfx.
 */
class Renderer {
protected:
    const Application* m_parentApp;
    uint32_t m_width, m_height;
    float m_viewMatrix[16];
    float m_projMatrix[16];
    bgfx::ProgramHandle m_spriteProgram;
    bgfx::UniformHandle m_sampler;
    bgfx::VertexLayout m_vertexLayout;
    std::vector<Drawable> m_queue;
    bool m_initialized;

    void submitBatch(std::vector<math::Vertex>& vertices, Texture* texture) const;
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    void init(const Application& parentApp, uint32_t width, uint32_t height, bool vsync, bgfx::RendererType::Enum rendererType);

    /** @return The type of renderer being used. */
    [[nodiscard]] bgfx::RendererType getType();

    /** Controls VSync. See @ref Application for more info. */
    void setVsync(bool vsync) const;

    void resize(uint32_t width, uint32_t height);

    static void setClearColor(Color color);

    void queueDrawable(const Drawable& drawable);

    void renderFrame();

    void shutdown();
};

}
