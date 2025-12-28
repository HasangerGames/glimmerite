
#include <filesystem>
#include <fstream>

#include <glaze/core/context.hpp>
#include <glaze/json/read.hpp>

#include "bimg/decode.h"

#include "gmi/client/TextureManager.h"
#include "gmi/client/gmi.h"
#include "gmi/math/Rect.h"

namespace gmi {

void TextureManager::load(
    const std::string& name,
    const std::string& filePath,
    bool isPixelArt,
    uint32_t width,
    uint32_t height
) {
    if (m_textures.contains(name)) {
        throw GmiException("Failed to load texture '" + name + "': Texture already exists");
    }

    // Create a placeholder 1x1 texture if it doesn't exist
    // We have to do this here because bgfx isn't initialized yet in the constructor
    if (!bgfx::isValid(m_placeholderHandle)) {
        uint32_t white = 0xffffffff;
        m_placeholderHandle = bgfx::createTexture2D(
            1,
            1,
            false,
            1u,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE,
            bgfx::copy(&white, sizeof(white))
        );
    }

    // Use the placeholder texture until the actual texture gets loaded
    m_textures[name] = {
        .handle = m_placeholderHandle,
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

    loadFile(
        filePath,
        [this, name, isPixelArt](const Buffer& data) {
            bimg::ImageContainer* image = bimg::imageParse(&m_allocator, data.data(), data.size());
            uint32_t width = image->m_width;
            uint32_t height = image->m_height;
            bgfx::TextureHandle handle = bgfx::createTexture2D(
                width,
                height,
                image->m_numMips > 1,
                1u,
                static_cast<bgfx::TextureFormat::Enum>(image->m_format),
                isPixelArt ? BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT : BGFX_TEXTURE_NONE,
                bgfx::copy(image->m_data, image->m_size)
            );
            bimg::imageFree(image);

            if (!bgfx::isValid(handle)) {
                throw GmiException("Failed to load texture '" + name + "': Texture is not valid");
            }

            m_handles.push_back(handle);
            Texture& texture = m_textures[name];
            texture.handle = handle;
            texture.frame.w = texture.size.w = width;
            texture.frame.h = texture.size.h = height;
            texture.loading = false;

            for (Container* container : m_pendingTextureUpdates[name]) {
                container->setDirty();
            }
            m_pendingTextureUpdates.erase(name);
        },
        [name, filePath] {
            throw GmiException("Failed to load texture '" + name + "': File not found: " + filePath);
        }
    );
}

void TextureManager::load(const std::string &filePath, bool isPixelArt) {
    load(std::filesystem::path(filePath).stem().string(), filePath, isPixelArt);
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
    load(name, sheetPath, sheet.meta.size.w * sheet.meta.scale, sheet.meta.size.h * sheet.meta.scale);
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

void TextureManager::updateOnLoad(const std::string& name, Container* container) {
    if (m_pendingTextureUpdates.contains(name)) {
        m_pendingTextureUpdates[name].push_back(container);
    } else {
        m_pendingTextureUpdates[name] = {container};
    }
}

void TextureManager::destroyAll() const {
    for (const bgfx::TextureHandle& handle : m_handles) {
        bgfx::destroy(handle);
    }
    // bgfx::destroy(m_placeholderHandle); TODO Figure out why this handle is invalid here (does it get destroyed elsewhere?)
}

}
