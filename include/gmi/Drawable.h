#pragma once

#include "bgfx/bgfx.h"
#include "math/Vertex.h"

#include <vector>

namespace gmi {

struct Drawable {
    std::vector<math::Vertex> vertices;
    std::vector<uint16_t> indices;
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
};

}
