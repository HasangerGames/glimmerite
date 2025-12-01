#pragma once
#include <functional>
#include <memory>
#include <string>

#include "Backend.h"
#include "Container.h"
#include "math/Size.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_init.h"
#include "Color.h"
#include "Tween.h"

namespace gmi {

/** Backends supported by Glimmerite. See @ref Backend for more info. */
enum class BackendType {
    Sdl,
    Bgfx,
    Auto
};

/** Renderers supported by Glimmerite. */
enum class RendererType {
    Metal,
    Direct3d12,
    Direct3d11,
    Vulkan,
    OpenGl,
    OpenGlEs,
    Software,
    Auto,
    Unknown
};

/**
 * If a @ref Backend is initialized with RendererType::Auto,
 * or the provided RendererType is not supported by the system or the Backend,
 * the first supported RendererType in this list will be chosen automatically.
 */
constexpr RendererType PREFERRED_RENDERERS[]{
    RendererType::Metal,
    RendererType::Direct3d12,
    RendererType::Direct3d11,
    RendererType::Vulkan,
    RendererType::OpenGl,
    RendererType::OpenGlEs,
    RendererType::Software
};

/** Configuration options used when creating an Application instance. */
struct ApplicationConfig {
    /** Initial width of the Application window. */
    int width{800};
    /** Initial height of the Application window. */
    int height{600};

    /** Title of the Application window. */
    std::string title{"Glimmerite Application"};

    /** Background color of the Application window. */
    Color backgroundColor{Color::fromRgb(0, 0, 0)};

    /**
     * The @ref Renderer to use.
     *
     * Not all renderers are available on all platforms.
     * If the specified renderer is not available, Glimmerite will automatically fall back to a different one.
     */
    RendererType renderer{RendererType::Auto};

    /** The @ref Backend to use. */
    BackendType backend{BackendType::Auto};

    /**
     * Syncs the framerate of the Application to the monitor's refresh rate.
     * Recommended to reduce resource usage and screen tearing.
     */
    bool vsync = true;
};

class Application {
    SDL_Window* m_window;
    std::unique_ptr<Backend> m_backend;

    uint16_t m_maxFps{0};
    std::chrono::time_point<std::chrono::steady_clock> m_lastFrame;
    float m_dt{0.0f};

    std::vector<std::function<void()>> m_tickers;
    std::unordered_map<Uint32, std::function<void(const SDL_Event&)>> m_eventListeners;
    TweenManager m_tweenManager;

    Container m_stage;
public:
    /**
     * Creates a new Application.
     * @param config The configuration to use
     */
    explicit Application(const ApplicationConfig& config);
    ~Application() = default;

    /** @return The root Container of the Application */
    Container& getStage() { return m_stage; }

    TweenManager& getTweenManager() { return m_tweenManager; }

    /**
     * Registers a function to be called every frame.
     * @param ticker The ticker function
     */
    void addTicker(const std::function<void()>& ticker);

    /**
     * Registers a function to listen for an event.
     * If a listener was previously registered for the given event, that listener will be overwritten by this one.
     * @param event The event to listen for
     * @param listener The function to be called when the event is triggered
     */
    void addEventListener(const SDL_EventType event, const std::function<void(const SDL_Event&)>& listener) { m_eventListeners[event] = listener; }

    /**
     * Loads a @ref Texture from disk.
     * @param filePath The path to the texture file to load
     * @return A pointer to the newly created texture
     */
    [[nodiscard]] Texture& loadTexture(const std::string& filePath) const;

    /**
     * Sets the frame limit. In most cases, it is recommended to use VSync instead, which is enabled by default.
     * If set to 0 or lower, this frame limit will be disabled, although VSync will continue to limit framerate if enabled.
     * @param fps Maximum frames per second
     */
    void setMaxFps(const uint16_t fps) { m_maxFps = fps; }

    /**
     * Controls VSync, which syncs the framerate to the monitor's refresh rate.
     * VSync is enabled by default. When enabled, it reduces screen tearing and resource usage.
     * @param vsync Whether VSync should be enabled
     */
    void setVsync(const bool vsync) const { m_backend->setVsync(vsync); }

    /** @return Delta time (time elapsed since previous frame) in milliseconds. */
    [[nodiscard]] float getDt() const { return m_dt; }

    /** @return The @ref Backend associated with this Application */
    [[nodiscard]] Backend& getBackend() const { return *m_backend; }

    /** @return The Size of the Application window */
    [[nodiscard]] math::Size getSize() const;

    /** @param size The Size to set the Application window to */
    void setSize(math::Size size) const;

    /**
     * @param width The width to set the Application window to
     * @param height The height to set the Application window to
     */
    void setSize(int width, int height) const;

    /** @return The window object backing this Application, or nullptr if it hasn't been initialized yet */
    [[nodiscard]] SDL_Window* getWindow() const { return m_window; }

    /** This method is called internally once per frame and should never be called manually. */
    SDL_AppResult iterate();

    /** This method is called internally when an event is received and should never be called manually. */
    SDL_AppResult processEvent(const SDL_Event* event);

    /** This method is called internally when the program terminates and should never be called manually. */
    void shutdown(SDL_AppResult result);
};

}
