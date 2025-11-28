#include "backends/bgfx/BgfxBackend.h"
#include "backends/sdl/SdlBackend.h"
#include "gmi/Application.h"

namespace gmi::internal {

RendererType sdlRendererNameToType(const std::string& name) {
    if (name == "vulkan")     return RendererType::Vulkan;
    if (name == "direct3d11") return RendererType::Direct3d11;
    if (name == "direct3d12") return RendererType::Direct3d12;
    if (name == "metal")      return RendererType::Metal;
    if (name == "opengl")     return RendererType::OpenGl;
    if (name == "opengles2")  return RendererType::OpenGlEs;
    if (name == "software")   return RendererType::Software;
    return RendererType::Unknown;
}

std::unique_ptr<Backend> createBackend(Application& parentApp, RendererType rendererType, BackendType backendType) {
    // default to bgfx
    if (backendType == BackendType::Auto) {
        backendType = BackendType::Bgfx;
    }

    switch (backendType) {
        case BackendType::Sdl:
            return std::make_unique<SdlBackend>(parentApp, rendererType);
        case BackendType::Bgfx:
            return std::make_unique<BgfxBackend>(parentApp, rendererType);
        default: // This should never happen!
            return nullptr;
    }
}

}
