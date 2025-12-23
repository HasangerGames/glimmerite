#define SDL_MAIN_USE_CALLBACKS 1

#include "SDL3/SDL_main.h"

#include "gmi/client/Application.h"

#include <thread>

#include "gmi/client/gmi.h"

using namespace gmi;

void gmiMain(Application& app);

SDL_AppResult SDL_AppInit(void** appstate, int /*argc*/, char* /*argv*/[]) {
    auto* app = new Application();
    gmiMain(*app);
    if (!app->isInitialized()) {
        throw GmiException("Application must be initialized within gmiMain");
    }
    *appstate = app;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    return static_cast<Application*>(appstate)->processEvent(event);
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return static_cast<Application*>(appstate)->iterate();
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = static_cast<Application*>(appstate);
    app->shutdown(result);
    delete app;
}

using namespace std::chrono;

namespace gmi {

void Application::init(const ApplicationConfig& config) {
    if (m_initialized) {
        throw GmiException("Application has already been initialized");
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        throw GmiException(std::string{"Unable to initialize SDL: "} + SDL_GetError());
    }

    SDL_WindowFlags flags = 0;
    if (config.resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    m_window = SDL_CreateWindow(config.title.c_str(), config.width, config.height, flags);
    if (m_window == nullptr) {
        throw GmiException(std::string{"Unable to create window: "} + SDL_GetError());
    }

    m_renderer.init(*this, config.width, config.height, config.vsync, config.renderer);
    Renderer::setClearColor(config.backgroundColor);

    m_soundManager.init();

    m_initialized = true;
}

void Application::addTicker(const std::function<void()>& ticker) {
    m_tickers.push_back(ticker);
}

math::Size Application::getSize() const {
    math::Size size;
    SDL_GetWindowSize(m_window, &size.width, &size.height);
    return size;
}

void Application::setSize(math::Size size) const {
    SDL_SetWindowSize(m_window, size.width, size.height);
}

void Application::setSize(int width, int height) const {
    SDL_SetWindowSize(m_window, width, height);
}

SDL_AppResult Application::processEvent(SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        m_renderer.resize(event->window.data1, event->window.data2);
    }

    if (m_eventListeners.contains(event->type)) {
        m_eventListeners[event->type](*event);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::iterate() {
    time_point<steady_clock> frameStart = steady_clock::now();
    if (!m_firstRun) {
        m_dt = duration<float, std::milli>(steady_clock::now() - m_lastFrame).count();
    } else {
        m_firstRun = false;
    }
    m_lastFrame = frameStart;

    for (const auto& ticker : m_tickers)
        ticker();
    m_tweenManager.update();
    m_renderer.render(m_stage);

    if (m_maxFps > 0) {
        float elapsed = duration<float, std::milli>(steady_clock::now() - frameStart).count();
        float idealDt = 1000.0f / static_cast<float>(m_maxFps);
        if (elapsed < idealDt) {
            std::this_thread::sleep_for(duration<float, std::milli>(idealDt - elapsed));
        }
    }

    return SDL_APP_CONTINUE;
}

void Application::shutdown(SDL_AppResult /*result*/) const {
    m_textureManager.destroyAll();
    m_renderer.shutdown();
}

}
