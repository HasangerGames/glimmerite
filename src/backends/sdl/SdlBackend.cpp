#include "gmi/backends/sdl/SdlBackend.h"

#include "gmi/Application.h"
#include "gmi/util/Color.h"
#include "SDL3/SDL_render.h"

namespace gmi {

SdlBackend::SdlBackend(Application* parentApp, GraphicsApi graphicsApi) : Backend(parentApp) {
    m_renderer = SDL_CreateRenderer(parentApp->getWindow(), );
}

void SdlBackend::setClearColor(const Color color) {
    SDL_SetRenderDrawColorFloat(m_renderer, color.r, color.g, color.b, color.a);
}

}
