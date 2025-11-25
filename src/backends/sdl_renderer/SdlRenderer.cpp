#include "gmi/renderers/sdl/SdlRenderer.h"

#include "gmi/Application.h"
#include "gmi/util/Color.h"
#include "SDL3/SDL_render.h"

namespace gmi {

SdlRenderer::SdlRenderer(Application* parentApp, GraphicsApi graphicsApi) : Renderer(parentApp) {
    m_renderer = SDL_CreateRenderer(parentApp->getWindow(), );
}

void SdlRenderer::setClearColor(const Color color) {
    SDL_SetRenderDrawColorFloat(m_renderer, color.r, color.g, color.b, color.a);
}

}
