#pragma once
#include "math/Vertex.h"
#include "gmi/TextureManager.h"
#include <vector>

namespace gmi {

struct Drawable {
    std::vector<math::Vertex> vertices;
    bgfx::TextureHandle* texture;
};

}
