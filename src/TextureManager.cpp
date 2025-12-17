#include "gmi/TextureManager.h"

#include <filesystem>
#include <fstream>
#include <glaze/core/context.hpp>
#include <glaze/json/read.hpp>
#include <iostream>

#include "bimg/decode.h"
#include "gmi/gmi.h"
#include "gmi/math/Rect.h"

namespace gmi {

void TextureManager::load(const std::string& name, const std::string& filePath) {
    if (m_textures.contains(name)) {
        throw GmiException("Failed to load texture '" + name + "': Texture already exists");
    }

    auto stream = std::ifstream(filePath, std::ios::binary);
    if (!stream.good()) {
        throw GmiException("Failed to load texture '" + name + "': File not found: " + filePath);
    }

    stream.seekg(0, std::ios::end);
    std::streamsize size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    char data[size];
    stream.read(data, size);

    bimg::ImageContainer* image = bimg::imageParse(&m_allocator, data, size);
    uint32_t width = image->m_width;
    uint32_t height = image->m_height;
    bgfx::TextureHandle handle = bgfx::createTexture2D(
        width,
        height,
        image->m_numMips > 1,
        1u,
        static_cast<bgfx::TextureFormat::Enum>(image->m_format),
        BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
        bgfx::copy(image->m_data, image->m_size)
    );
    bimg::imageFree(image);

    if (!bgfx::isValid(handle)) {
        throw GmiException("Failed to load texture '" + name + "': Texture is not valid");
    }

    m_handles.push_back(handle);
    m_textures[name] = {
        .handle = handle,
        .size = {
            .w = width,
            .h = height
        },
        .frame = {
            .x = 0,
            .y = 0,
            .w = width,
            .h = height,
        }
    };
}

void TextureManager::load(const std::string& filePath) {
    load(std::filesystem::path(filePath).stem().string(), filePath);
}

struct SpritesheetFrame {
    math::UintRect frame;
    math::UintSize sourceSize;
};

struct SpritesheetMeta {
    std::string image;
    float scale;
    math::UintSize size;
};

struct Spritesheet {
    SpritesheetMeta meta;
    std::unordered_map<std::string, SpritesheetFrame> frames;
};

void TextureManager::loadSpritesheet(const std::string& name, const std::string& filePath) {
    auto dataFile = std::ifstream(filePath);
    if (!dataFile.good()) {
        throw GmiException("Failed to parse spritesheet '" + name + "': File not found: " + filePath);
    }

    std::ostringstream stream;
    stream << dataFile.rdbuf();
    std::string sheetData = stream.str();

    Spritesheet sheet{};
    if (const glz::error_ctx err = glz::read_json(sheet, sheetData)) {
        throw GmiException("Failed to parse spritesheet '" + name + "': " + glz::format_error(err, sheetData));
    }

    std::string sheetPath = (std::filesystem::path{filePath}.parent_path() / sheet.meta.image).string();
    load(name, sheetPath);
    Texture texture = m_textures[name];

    for (const auto& [subName, frame] : sheet.frames) {
        m_textures[subName] = {
            .handle = texture.handle,
            .size = texture.size,
            .frame = frame.frame,
        };
    }
}

void TextureManager::loadSpritesheet(const std::string& filePath) {
    loadSpritesheet(std::filesystem::path(filePath).string(), filePath);
}

Texture& TextureManager::get(const std::string& name) {
    if (!m_textures.contains(name)) {
        throw GmiException("Texture not found: '" + name + "'");
    }
    return m_textures[name];
}

void TextureManager::destroyAll() const {
    for (const bgfx::TextureHandle& handle : m_handles) {
        bgfx::destroy(handle);
    }
}

}
