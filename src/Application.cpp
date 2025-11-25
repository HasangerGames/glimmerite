#include "gmi/Application.h"

#include "gmi/renderers/sdl/SdlRenderer.h"
#include "SDL3/SDL_init.h"

namespace gmi {

Application::~Application() {
    SDL_DestroyWindow(m_window);
    // we don't SDL_Quit() to avoid interrupting other things SDL might be doing
    // by contrast, Application::shutdown() is explicitly meant to be called when the program terminates,
    // so SDL_Quit() is fine there
}

bool Application::init(const ApplicationConfig& config) {
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow(config.title.c_str(), config.width, config.height, SDL_WINDOW_RESIZABLE);
    m_renderer = std::make_unique<SdlRenderer>(this);
    return m_window != nullptr;
}

void Application::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_closeRequested = true;
        }
    }
}

void Application::shutdown() const {
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Application::closeRequested() const {
    return m_closeRequested;
}

Renderer& Application::getRenderer() const {
    return *m_renderer;
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

SDL_Window* Application::getWindow() const {
    return m_window;
}

}
