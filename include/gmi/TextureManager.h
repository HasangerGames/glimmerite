#pragma once
#include <string>
#include <unordered_map>

#include "bgfx/bgfx.h"

namespace gmi {

struct Texture {
    bgfx::TextureHandle handle;
    uint32_t width;
    uint32_t height;
};

class TextureManager {
    std::unordered_map<std::string, Texture> m_textures;
public:
    /**
     * Loads a @ref Texture from disk.
     * @param name The name to give the texture
     * @param filePath The path to the texture file to load
     */
    void load(const std::string& name, const std::string& filePath);

    /** @return The @ref Texture with the given name */
    Texture& get(const std::string& name);

    void destroyAll();
};

}
