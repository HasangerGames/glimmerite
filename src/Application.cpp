#include "gmi/Application.h"

#include <iostream>
#include <thread>

#include "backends/sdl/SdlBackend.h"
#include "gmi/gmi.h"
#include "internal/utils.h"
#include "SDL3/SDL_init.h"

using namespace std::chrono;

namespace gmi {

Application::Application(const ApplicationConfig& config) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw GmiException(std::string{"Unable to initialize SDL: "} + SDL_GetError());
    }

    m_window = SDL_CreateWindow(config.title.c_str(), config.width, config.height, SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr) {
        throw GmiException(std::string{"Unable to create window: "} + SDL_GetError());
    }

    m_backend = internal::createBackend(*this, config.renderer, config.backend);
    if (m_backend == nullptr) {
        throw GmiException("Unable to initialize backend");
    }

    m_backend->setClearColor(config.backgroundColor);
    m_backend->setVsync(config.vsync);
}

Application::~Application() {
    SDL_DestroyWindow(m_window);
    // we don't SDL_Quit() to avoid interrupting other things SDL might be doing
}

void Application::start() {
    while (!m_closeRequested) {
        mainLoop();
    }
    shutdown();
}

void Application::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_closeRequested = true;
        }
        if (m_eventListeners.contains(event.type)) {
            m_eventListeners[event.type](event);
        }
    }
}

void Application::mainLoop() {
    const auto frameStart{steady_clock::now()};
    m_dt = duration<float, std::milli>(steady_clock::now() - m_lastFrame).count();
    m_lastFrame = frameStart;

    pollEvents();
    m_ticker();
    m_stage.render(*m_backend, {});
    m_backend->renderFrame();

    if (m_maxFps > 0) {
        const float elapsed{duration<float, std::milli>(steady_clock::now() - frameStart).count()};
        const float idealDt{1000.0f / static_cast<float>(m_maxFps)};
        if (elapsed < idealDt) {
            std::this_thread::sleep_for(duration<float, std::milli>(idealDt - elapsed));
        }
    }
}

void Application::shutdown() const {
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

Texture& Application::createTexture(const std::string &filePath) const {
    return m_backend->createTexture(filePath);
}

math::Size Application::getSize() const {
    math::Size size;
    SDL_GetWindowSize(m_window, &size.width, &size.height);
    return size;
}

void Application::setSize(const math::Size size) const {
    SDL_SetWindowSize(m_window, size.width, size.height);
}

void Application::setSize(const int width, const int height) const {
    SDL_SetWindowSize(m_window, width, height);
}

}
