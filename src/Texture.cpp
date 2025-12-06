#include "gmi/Texture.h"

#include <fstream>
#include <iostream>

#include "bimg/decode.h"
#include "bx/allocator.h"
#include "gmi/gmi.h"

namespace gmi {

Texture::Texture(const std::string& filePath) {
    std::ifstream stream(filePath, std::ios::binary);
    // TODO Check if file exists

    stream.seekg(0, std::ios::end);
    const std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    char data[size];
    stream.read(data, size);

    bx::DefaultAllocator allocator;
    bimg::ImageContainer* image = bimg::imageParse(&allocator, data, size);
    m_width = image->m_width;
    m_height = image->m_height;
    m_textureHandle = bgfx::createTexture2D(
        m_width,
        m_height,
        image->m_numMips > 1,
        1u,
        static_cast<bgfx::TextureFormat::Enum>(image->m_format),
        BGFX_TEXTURE_NONE,
        bgfx::copy(image->m_data, image->m_size)
    );
    bimg::imageFree(image);

    if (!bgfx::isValid(m_textureHandle)) {
        throw GmiException("Failed to load texture " + filePath + ": Texture is not valid");
    }
}

}
