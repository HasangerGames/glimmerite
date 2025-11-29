#pragma once
#include "Transform.h"
#include "Vertex.h"
#include "gmi/Texture.h"
#include <vector>

#include "Affine.h"

namespace gmi::math {

struct Geometry {
    Affine affine;
    Color color;
    Texture* texture;
    std::vector<Vertex> vertices;
};

}
