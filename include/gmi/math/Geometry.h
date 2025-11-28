#pragma once
#include "Transform.h"
#include "Vertex.h"
#include "gmi/Texture.h"

namespace gmi::math {

struct Geometry {
    Transform transform;
    Texture* texture = nullptr;
    std::vector<Vertex> vertices;
};

}
