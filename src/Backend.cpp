#include "backends/bgfx/BgfxBackend.h"
#include "gmi/Application.h"

namespace gmi {

Backend::Backend(const Application& parentApp, const RendererType rendererType) : m_parentApp(parentApp), m_rendererType(rendererType) {}

void Backend::queueGeometry(const math::Geometry& geometry) {
    m_queue.push_back(geometry);
}

}
