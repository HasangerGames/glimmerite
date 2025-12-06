#pragma once
#include <string>

#include "bgfx/bgfx.h"

namespace gmi {

class Texture {
    bgfx::TextureHandle m_textureHandle{};
    uint32_t m_width;
    uint32_t m_height;
public:
    explicit Texture(const std::string& filePath);
    ~Texture() = default;

    /** @return The width of this Texture in pixels */
    uint32_t getWidth() { return m_width; }

    /** @return The height of this Texture in pixels */
    uint32_t getHeight() { return m_height; }

    /** @return A pointer to the bgfx texture object backing this Texture */
    bgfx::TextureHandle& getRawTexture() { return m_textureHandle; }
};

}
