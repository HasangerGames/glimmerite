#pragma once
#include "gmi/Application.h"
#include "gmi/Backend.h"
#include "SDL3/SDL_render.h"

namespace gmi {

class SdlBackend final : public Backend {
    SDL_Renderer* m_renderer;
    RendererType m_rendererType;
    uint16_t m_maxFps{0};
public:
    SdlBackend(const Application& parentApp, RendererType rendererType);

    [[nodiscard]] BackendType getType() const override { return BackendType::Sdl; }

    void setVsync(bool vsync) override;

    [[nodiscard]] Texture& loadTexture(const std::string& filename) override;

    void renderFrame() override;
    void setClearColor(Color color) override;
};

}
