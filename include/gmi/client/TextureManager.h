#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "bgfx/bgfx.h"
#include "bx/allocator.h"

#include "gmi/math/Rect.h"
#include "gmi/math/Size.h"

namespace gmi {

struct Texture {
    bgfx::TextureHandle handle;
    math::UintSize size;
    math::UintRect frame;
};

class TextureManager {
public:
    /**
     * Loads a @ref Texture from disk.
     * @param name The name to give the texture
     * @param filePath The path to the texture file to load
     */
    void load(const std::string& name, const std::string& filePath);

    /**
     * Loads a @ref Texture from disk.
     * The name will be assigned automatically based on the filename.
     * For example, `assets/test.png` will result in a texture named `test`.
     * @param filePath The path to the texture file to load
     */
    void load(const std::string& filePath);

    /**
     * Loads a spritesheet from disk, adding all its textures.
     * This method expects a JSON file of the type outputted by tools like [TexturePacker](https://www.codeandweb.com/texturepacker).
     * @param name The name to give the spritesheet texture
     * @param filePath The path to the spritesheet JSON file
     */
    void loadSpritesheet(const std::string& name, const std::string& filePath);

    /**
     * Loads a spritesheet from disk, adding all its textures.
     * This method expects a JSON file of the type outputted by tools like [TexturePacker](https://www.codeandweb.com/texturepacker).
     * @param filePath The path to the spritesheet JSON file
     */
    void loadSpritesheet(const std::string& filePath);

    /** @return The @ref Texture with the given name */
    Texture& get(const std::string& name);

    /** Destroys all textures belonging to this TextureManager. */
    void destroyAll() const;
private:
    bx::DefaultAllocator m_allocator;
    std::vector<bgfx::TextureHandle> m_handles;
    std::unordered_map<std::string, Texture> m_textures;
};

}
