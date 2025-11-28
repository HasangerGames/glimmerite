#include <set>

#include "bgfx/bgfx.h"
#include "gmi/Application.h"
#include "SDL3/SDL_render.h"
#include "internal/utils.h"

namespace gmi {

std::set<RendererType> getSupportedRenderers(BackendType backend = BackendType::Auto) {
    // default to bgfx
    if (backend == BackendType::Auto) {
        backend = BackendType::Bgfx;
    }

    std::set<RendererType> renderers;
    switch (backend) {
        case BackendType::Bgfx: {
            bgfx::RendererType::Enum rendererTypes[bgfx::RendererType::Count];
            const uint8_t numRenderers{bgfx::getSupportedRenderers(bgfx::RendererType::Count, rendererTypes)};
            for (int i = 0; i < numRenderers; i++) {
                RendererType rendererType;

                switch (rendererTypes[i]) {
                    case bgfx::RendererType::Vulkan:     rendererType = RendererType::Vulkan; break;
                    case bgfx::RendererType::Direct3D11: rendererType = RendererType::Direct3d11; break;
                    case bgfx::RendererType::Direct3D12: rendererType = RendererType::Direct3d12; break;
                    case bgfx::RendererType::Metal:      rendererType = RendererType::Metal; break;
                    case bgfx::RendererType::OpenGL:     rendererType = RendererType::OpenGl; break;
                    case bgfx::RendererType::OpenGLES:   rendererType = RendererType::OpenGlEs; break;
                    default:                             rendererType = RendererType::Unknown; break;
                }

                if (rendererType != RendererType::Unknown) {
                    renderers.insert(rendererType);
                }
            }
            break;
        }
        case BackendType::Sdl: {
            const int numRenderers{SDL_GetNumRenderDrivers()};
            for (int i = 0; i < numRenderers; i++) {
                const RendererType rendererType{internal::sdlRendererNameToType(SDL_GetRenderDriver(i))};
                if (rendererType != RendererType::Unknown) {
                    renderers.insert(rendererType);
                }
            }
            break;
        }
        default: break;
    }
    return renderers;
}

std::string getRendererName(const RendererType rendererType) {
    switch (rendererType) {
        case RendererType::Vulkan:     return "Vulkan";
        case RendererType::Direct3d11: return "Direct3D 11";
        case RendererType::Direct3d12: return "Direct3D 12";
        case RendererType::Metal:      return "Metal";
        case RendererType::OpenGl:     return "OpenGL";
        case RendererType::OpenGlEs:   return "OpenGL ES";
        case RendererType::Software:   return "Software";
        case RendererType::Auto:       return "Auto";
        default:                       return "Unknown";
    }
}

}
