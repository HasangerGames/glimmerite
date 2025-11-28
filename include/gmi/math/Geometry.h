#pragma once
#include "Transform.h"
#include "Vertex.h"
#include "gmi/Texture.h"
#include <vector>

namespace gmi::math {

struct Geometry {
    Transform transform;
    Texture* texture;
    std::vector<Vertex> vertices;
};

}
