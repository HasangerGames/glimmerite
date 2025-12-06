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
 * The Backend is an API which handles communication between the Application and bgfx.
 */
class Backend {
protected:
    const Application* m_parentApp;
    uint32_t m_width, m_height;
    float m_viewMatrix[16];
    float m_projMatrix[16];
    bgfx::ProgramHandle m_spriteProgram;
    bgfx::UniformHandle m_sampler;
    bgfx::VertexLayout m_vertexLayout;
    std::deque<Texture> m_textures;
    std::vector<Drawable> m_queue;
    bool m_initialized;

    void submitBatch(std::vector<math::Vertex>& vertices, Texture* texture);
public:
    Backend() = default;
    virtual ~Backend() = default;

    void init(const Application& parentApp, uint32_t width, uint32_t height, bgfx::RendererType::Enum rendererType);

    /** @return The type of renderer being used by this Backend. */
    [[nodiscard]] bgfx::RendererType getRendererType();

    /** Controls VSync. See @ref Application for more info. */
    void setVsync(bool vsync) const;

    void resize(uint32_t width, uint32_t height);

    /** Loads a texture from disk. See @ref Application for more info. */
    [[nodiscard]] Texture& loadTexture(const std::string& filePath);

    static void setClearColor(Color color);

    void queueDrawable(const Drawable& drawable);

    void renderFrame();

    void shutdown();
};

}
