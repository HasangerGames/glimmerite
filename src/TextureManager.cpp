#include "gmi/TextureManager.h"

#include <fstream>
#include <iostream>
#include <ranges>

#include "bimg/decode.h"
#include "bx/allocator.h"
#include "gmi/gmi.h"

namespace gmi {

void TextureManager::load(const std::string& name, const std::string& filePath) {
    if (m_textures.contains(name)) {
        throw GmiException("Failed to load texture '" + name + "': Texture already exists");
    }

    std::ifstream stream(filePath, std::ios::binary);
    // TODO Check if file exists

    stream.seekg(0, std::ios::end);
    const std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    char data[size];
    stream.read(data, size);

    bx::DefaultAllocator allocator;
    bimg::ImageContainer* image = bimg::imageParse(&allocator, data, size);
    const uint32_t width = image->m_width;
    const uint32_t height = image->m_height;
    const bgfx::TextureHandle handle = bgfx::createTexture2D(
        width,
        height,
        image->m_numMips > 1,
        1u,
        static_cast<bgfx::TextureFormat::Enum>(image->m_format),
        BGFX_TEXTURE_NONE,
        bgfx::copy(image->m_data, image->m_size)
    );
    m_textures[name] = {
        .handle = handle,
        .width = image->m_width,
        .height = image->m_height
    };
    bimg::imageFree(image);

    if (!bgfx::isValid(handle)) {
        throw GmiException("Failed to load texture '" + name + "': Texture is not valid");
    }
}

Texture& TextureManager::get(const std::string& name) {
    if (!m_textures.contains(name)) {
        throw GmiException("Texture not found: '" + name + "'");
    }
    return m_textures[name];
}

void TextureManager::destroyAll() {
    for (const Texture& texture : m_textures | std::views::values) {
        bgfx::destroy(texture.handle);
    }
}

}
