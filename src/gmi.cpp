#include <vector>

#include "bgfx/bgfx.h"
#include "gmi/Application.h"

namespace gmi {

std::vector<GraphicsApi> getSupportedRenderers(BackendType backend = BackendType::Auto) {
    // default to bgfx
    if (backend == BackendType::Auto) {
        backend = BackendType::Bgfx;
    }

    std::vector<GraphicsApi> apis;
    switch (backend) {
        case BackendType::Bgfx:
            bgfx::RendererType::Enum rendererTypes[bgfx::RendererType::Count]; // Cannot resolve symbol 'bgfx'
            break;
    }
    return apis;
}

}
