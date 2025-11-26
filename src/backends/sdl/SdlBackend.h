#pragma once
#include "gmi/Application.h"
#include "gmi/Backend.h"
#include "SDL3/SDL_render.h"

namespace gmi {

class SdlBackend : public Backend {
    SDL_Renderer* m_renderer;
    GraphicsApi m_graphicsApi;
public:
    explicit SdlBackend(Application* parentApp);

    void init() override;

    void renderFrame() override;
    void setClearColor(Color color) override;
};

}
