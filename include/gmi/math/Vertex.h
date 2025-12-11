#pragma once

#include "Affine.h"
#include <vector>

namespace gmi::math {

struct Vertex {
    float x, y;
    float u, v;
    uint32_t color;
};

inline std::vector<Vertex> transformVertices(const std::vector<Vertex>& vertices, const Affine& affine) {
    std::vector<Vertex> outVertices;
    outVertices.reserve(vertices.size());
    for (auto& [x, y, u, v, color] : vertices) {
        outVertices.emplace_back(
            (affine.a * x) + (affine.c * y) + affine.x,
            (affine.b * x) + (affine.d * y) + affine.y,
            u,
            v,
            color
        );
    }
    return outVertices;
}

}
