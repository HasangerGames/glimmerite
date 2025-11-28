#include "BgfxBackend.h"

namespace gmi {

BgfxBackend::BgfxBackend(const Application &parentApp, RendererType rendererType) : Backend(parentApp, rendererType) {}

Texture& BgfxBackend::createTexture(const std::string &filename) {
    m_textures.emplace_back();
    return *m_textures.back().get();
}

void BgfxBackend::setVsync(bool vsync) {

}

void BgfxBackend::renderFrame() {

}

void BgfxBackend::setClearColor(Color color) {

}

}
