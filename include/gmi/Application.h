#pragma once
#include <chrono>
#include <functional>
#include <string>

#include "Color.h"
#include "Container.h"
#include "Renderer.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include "SoundManager.h"
#include "TweenManager.h"
#include "gmi/TextureManager.h"
#include "math/Size.h"

namespace gmi {

/** Configuration options used when creating an Application instance. */
struct ApplicationConfig {
    /** Initial width of the Application window. */
    int width = 800;
    /** Initial height of the Application window. */
    int height = 600;

    /** Title of the Application window. */
    std::string title = "Glimmerite Application";

    /** Background color of the Application window. */
    Color backgroundColor = Color::Black;

    /** Whether the Application window can be resized. */
    bool resizable = true;

    /**
     * The renderer/graphics API to use.
     * Not all renderers are available on all platforms.
     * If the specified renderer is not available, Glimmerite will automatically fall back to a different one.
     */
    bgfx::RendererType::Enum renderer = bgfx::RendererType::Count;

    /**
     * Syncs the framerate of the Application to the monitor's refresh rate.
     * Recommended to reduce resource usage and screen tearing.
     */
    bool vsync = true;
};

class Application {
    bool m_initialized = false;

    SDL_Window* m_window = nullptr;

    uint16_t m_maxFps = 0;
    bool m_firstRun = true;
    std::chrono::time_point<std::chrono::steady_clock> m_lastFrame;
    float m_dt = 0.0f;

    std::vector<std::function<void()>> m_tickers;
    std::unordered_map<Uint32, std::function<void(const SDL_Event&)>> m_eventListeners;

    TextureManager m_textureManager;
    SoundManager m_soundManager;
    TweenManager m_tweenManager;
    Renderer m_renderer;
    Container m_stage;
public:
    Application() : m_stage(this, nullptr) {}
    ~Application() = default;

    /**
     * Initializes this Application.
     * @param config The configuration to use
     */
    void init(const ApplicationConfig& config);

    bool isInitialized() const { return m_initialized; }

    /** @return The @ref TextureManager associated with the Application, used to load textures */
    [[nodiscard]] TextureManager& textures() { return m_textureManager; }

    /** @return The @ref SoundManager associated with the Application, used to load and play sounds */
    [[nodiscard]] SoundManager& sounds() { return m_soundManager; }

    /** @return The @ref TweenManager associated with the Application, used to animate values */
    [[nodiscard]] TweenManager& tweens() { return m_tweenManager; }

    /** @return The @ref Renderer associated with the Application */
    [[nodiscard]] Renderer& renderer() { return m_renderer; }

    /** @return The root Container of the Application */
    Container& stage() { return m_stage; }

    /**
     * Controls VSync, which syncs the framerate to the monitor's refresh rate.
     * VSync is enabled by default. When enabled, it reduces screen tearing and resource usage.
     * @param vsync Whether VSync should be enabled
     */
    void setVsync(bool vsync) { m_renderer.setVsync(vsync); }

    /**
     * Sets the frame limit. In most cases, it is recommended to use VSync instead, which is enabled by default.
     * If set to 0 or lower, this frame limit will be disabled, although VSync will continue to limit framerate if enabled.
     * @param fps Maximum frames per second
     */
    void setMaxFps(uint16_t fps) { m_maxFps = fps; }

    /** @return Delta time (time elapsed since previous frame) in milliseconds. */
    [[nodiscard]] float getDt() const { return m_dt; }

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
    void addEventListener(SDL_EventType event, const std::function<void(const SDL_Event&)>& listener) { m_eventListeners[event] = listener; }

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
    SDL_AppResult processEvent(SDL_Event* event);

    /** This method is called internally when the program terminates and should never be called manually. */
    void shutdown(SDL_AppResult result) const;
};

}
