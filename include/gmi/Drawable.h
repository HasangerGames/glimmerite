#pragma once
#include "math/Vertex.h"
#include "gmi/Texture.h"
#include <vector>

namespace gmi {

struct Drawable {
    std::vector<math::Vertex> vertices;
    Texture* texture;
};

}
