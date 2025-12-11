#include "gmi/Graphics.h"

namespace gmi {

Graphics& Graphics::fillRect(const math::Rect& rect, Color color) {
    auto& [vertices, indices, texture] = m_drawable;

    size_t numVertices = vertices.size();
    indices.reserve(indices.size() + 6);
    indices.emplace_back(numVertices + 0);
    indices.emplace_back(numVertices + 1);
    indices.emplace_back(numVertices + 2);
    indices.emplace_back(numVertices + 0);
    indices.emplace_back(numVertices + 2);
    indices.emplace_back(numVertices + 3);

    auto [x, y, w, h] = rect;
    uint32_t colorNum = colorToNumber(color);
    vertices.reserve(numVertices + 4);
    vertices.emplace_back(x,     y,     0, 1, colorNum); // Top left
    vertices.emplace_back(x + w, y,     1, 1, colorNum); // Top right
    vertices.emplace_back(x + w, y + h, 1, 0, colorNum); // Bottom right
    vertices.emplace_back(x,     y + h, 0, 0, colorNum); // Bottom left

    return *this;
}

Graphics& Graphics::fillPoly(const std::vector<math::Vec2>& points, Color color) {
    auto& [vertices, indices, texture] = m_drawable;

    std::vector<uint16_t> polyIndices = mapbox::earcut<uint16_t>(std::vector<std::vector<math::Vec2>>{points});
    size_t numVertices = vertices.size();
    indices.reserve(indices.size() + polyIndices.size());
    for (uint16_t index : polyIndices) {
        indices.emplace_back(numVertices + index);
    }

    uint32_t colorNum = colorToNumber(color);
    vertices.reserve(numVertices + points.size());
    for (auto [x, y] : points) {
        vertices.emplace_back(x, y, 0, 0, colorNum);
    }

    return *this;
}

void Graphics::render(Renderer& renderer) {
    Container::render(renderer);

    renderer.queueDrawable(m_drawable);
}

}
