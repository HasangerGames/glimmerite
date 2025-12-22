#pragma once

#include "bgfx/bgfx.h"
#include "gmi/client/Vertex.h"

#include <vector>

namespace gmi {

struct Drawable {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
};

}
