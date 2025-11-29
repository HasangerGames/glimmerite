#include "backends/bgfx/BgfxBackend.h"

namespace gmi {

void Backend::queueGeometry(const math::Geometry& geometry) {
    m_queue.push_back(geometry);
}

}
