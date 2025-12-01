#pragma once
#include <chrono>
#include <vector>

#include "math/Geometry.h"
#include "Color.h"

namespace gmi {

class Application;
enum class BackendType;
enum class RendererType;

/**
 * A Backend is an API which takes geometry and renders it via a library like bgfx or SDL.
 *
 * Backends also handle things like batching, ensuring rendering happens fast with as few draw calls as possible.
 */
class Backend {
protected:
    const Application& m_parentApp;
    RendererType m_rendererType;

    std::vector<std::unique_ptr<Texture>> m_textures;

    std::vector<math::Geometry> m_queue;

    Backend(const Application& parentApp, const RendererType rendererType) : m_parentApp(parentApp), m_rendererType(rendererType) {}
public:
    virtual ~Backend() = default;

    /** @return The type of Backend this is. */
    [[nodiscard]] virtual BackendType getType() const = 0;

    /** @return The type of renderer being used by this Backend. */
    [[nodiscard]] RendererType getRendererType() const { return m_rendererType; }

    /** Controls VSync. See @ref Application for more info. */
    virtual void setVsync(bool vsync) = 0;

    /** Loads a texture from disk. See @ref Application for more info. */
    [[nodiscard]] virtual Texture& loadTexture(const std::string& filePath) = 0;

    virtual void renderFrame() = 0;

    virtual void setClearColor(Color color) = 0;

    void queueGeometry(const math::Geometry& geometry);
};

}
