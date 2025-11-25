#pragma once
#include "gmi/Application.h"
#include "gmi/Renderer.h"
#include "SDL3/SDL_render.h"

namespace gmi {

class SdlRenderer : public Renderer {
    SDL_Renderer* m_renderer;
    GraphicsApi m_graphicsApi;
public:
    explicit SdlRenderer(Application* parentApp);

    void init() override;

    void renderFrame() override;
    void setClearColor(Color color) override;
};

}
