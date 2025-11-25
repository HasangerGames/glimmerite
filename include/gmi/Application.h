#pragma once
#include <memory>
#include <string>

#include "Renderer.h"
#include "math/Size.h"
#include "SDL3/SDL_video.h"
#include "util/Color.h"

namespace gmi {

/** Renderers supported by Glimmerite. See @ref Renderer for more info. */
enum class RendererType {
    Sdl,
    Bgfx,
    Auto
};

/**
 * Graphics APIs supported by Glimmerite.
 * Not all APIs are available on all platforms.
 */
enum class GraphicsApi {
    Vulkan,
    Direct3d11,
    Direct3d12,
    Metal,
    OpenGl,
    OpenGlEs,
    Software,
    Auto
};

/** Configuration options used when creating an Application instance. */
struct ApplicationConfig {
    /** Initial width of the Application window. */
    int width = 800;
    /** Initial height of the Application window. */
    int height = 600;

    /** Title of the Application window. */
    std::string title = "Glimmerite Application";

    /** Background color of the Application window. */
    Color backgroundColor = Color::fromRgb(0, 0, 0);

    /** The renderer to use. See @ref Renderer for more information. */
    RendererType renderer{ RendererType::Auto };

    /**
     * The @ref GraphicsApi to use.
     *
     * Not all graphics APIs are available on all platforms.
     * If the specified API is not available, Glimmerite will automatically fall back to a different one.
     */
    GraphicsApi graphicsApi{ GraphicsApi::Auto };
};

class Application {
    SDL_Window* m_window;
    std::unique_ptr<Renderer> m_renderer;
    bool m_closeRequested = false;
public:
    /**
     * Creates an Application.
     * @param config The configuration to use
     */
    explicit Application() = default;
    ~Application();

    bool init(const ApplicationConfig& config);

    /**
     * Listens for input events.
     * This method must be called once per frame or inputs will not work properly.
     */
    void pollEvents();

    void shutdown() const;

    /**
     * @return Whether this Application should close.
     *         Set to true when the user clicks the close button on the window.
     */
    [[nodiscard]] bool closeRequested() const;

    /** @return The active Renderer associated with this Application */
    Renderer& getRenderer() const;

    /** @return The Size of the Application window */
    math::Size getSize() const;
    /** @param size The Size to set the Application window to */
    void setSize(math::Size size) const;
    /**
     * @param width The width to set the Application window to
     * @param height The height to set the Application window to
     */
    void setSize(int width, int height) const;

    /** @return The window object backing this Application */
    SDL_Window* getWindow() const;
};

}
