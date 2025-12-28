#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Container.h"
#include "bgfx/bgfx.h"
#include "bx/allocator.h"

#include "gmi/math/Rect.h"
#include "gmi/math/Size.h"

namespace gmi {

struct Texture {
    bgfx::TextureHandle handle;
    math::UintSize size;
    math::UintRect frame;
    bool loading = true;
};

class TextureManager {
public:
    /**
     * Loads a @ref Texture from disk.
     * @param name The name to give the texture
     * @param filePath The path to the texture file to load
     * @param isPixelArt Whether the texture is pixel art, using nearest-neighbor interpolation
     * @param width Default width of the texture to set while it's being loaded
     * @param height Default height of the texture to set while it's being loaded
     */
    void load(
        const std::string& name,
        const std::string& filePath,
        bool isPixelArt = false,
        uint32_t width = 1,
        uint32_t height = 1
    );

    /**
     * Loads a @ref Texture from disk.
     * The name will be assigned automatically based on the filename.
     * For example, `assets/test.png` will result in a texture named `test`.
     * @param filePath The path to the texture file to load
     * @param isPixelArt Whether the texture is pixel art, using nearest-neighbor interpolation
     */
    void load(const std::string& filePath, bool isPixelArt = false);

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

    /**
     * Triggers an update of a @ref Container when a texture finishes loading.
     * @param name The name of the texture to act as a trigger
     * @param container The container to update
     */
    void updateOnLoad(const std::string& name, Container* container);

    /** Destroys all textures belonging to this TextureManager. */
    void destroyAll() const;
private:
    bx::DefaultAllocator m_allocator;
    std::vector<bgfx::TextureHandle> m_handles;
    std::unordered_map<std::string, Texture> m_textures;
    std::unordered_map<std::string, std::vector<Container*>> m_pendingTextureUpdates;
    bgfx::TextureHandle m_placeholderHandle = BGFX_INVALID_HANDLE;
};

}
