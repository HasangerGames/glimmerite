#pragma once
#include "gmi/Application.h"
#include "gmi/Backend.h"

namespace gmi {

class BgfxBackend final : public Backend {
public:
    BgfxBackend(const Application& parentApp, RendererType rendererType);

    [[nodiscard]] BackendType getType() const override { return BackendType::Bgfx; }

    void setVsync(bool vsync) override;

    Texture& createTexture(const std::string& filename) override;

    void renderFrame() override;
    void setClearColor(Color color) override;
};

}
