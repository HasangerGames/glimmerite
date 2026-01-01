
#include <algorithm>
#include <filesystem>
#include <fstream>

#include <glaze/core/context.hpp>
#include <glaze/json/read.hpp>

#include "gmi/client/TextureManager.h"

#include "../../vendored/bgfx.cmake/bgfx/src/bgfx_p.h"
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
    ensureTextureDoesNotExist(name);

    initPlaceholderHandle();

    // Use the placeholder texture until the actual texture gets loaded
    m_textures[name] = {
        .handle = m_placeholderHandle,
        .size = {
            .w = width,
            .h = height,
        },
        .frame = {
            .x = 0,
            .y = 0,
            .w = width,
            .h = height,
        }
    };

    readFile(
        filePath,
        [this, name, isPixelArt](const Buffer& data) {
            auto [handle, w, h] = createTexture(data, name, isPixelArt);

            Texture& texture = m_textures[name];
            texture.handle = handle;
            texture.frame.w = texture.size.w = w;
            texture.frame.h = texture.size.h = h;
            texture.loading = false;

            releaseTextureUpdates(name);
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

using SpritesheetFrames = std::unordered_map<std::string, SpritesheetFrame>;

struct Spritesheet {
    SpritesheetMeta meta;
    SpritesheetFrames frames;
};

void TextureManager::loadSpritesheet(const std::string& filePath, bool isPixelArt) {
    loadSpritesheets({filePath}, isPixelArt);
}

void TextureManager::loadSpritesheets(const std::vector<std::string>& filePaths, bool isPixelArt) {
    std::unordered_map<std::string, Spritesheet> sheets;
    uint32_t width = 0;
    uint32_t height = 0;
    for (const std::string& path : filePaths) {
        std::string name = nameFromPath(path);
        ensureTextureDoesNotExist(name);

        Buffer data = readFileSync(path, "Failed to parse spritesheet '" + name + "'");
        auto sheetData = std::string(data.data());

        auto sheet = Spritesheet();
        if (const glz::error_ctx err = glz::read_json(sheet, sheetData)) {
            throw GmiException("Failed to parse spritesheet '" + name + "': " + glz::format_error(err, sheetData));
        }

        // Determine the dimensions of the texture array from the largest dimensions of the textures
        auto [w, h] = sheet.meta.size;
        width = std::max(w, width);
        height = std::max(h, height);

        sheets.emplace(path, sheet);
    }

    size_t numLayers = filePaths.size();
    if (numLayers > 255) {
        throw GmiException("Failed to load spritesheets: Cannot create more than 255 spritesheets");
    }
    bool supportsTextureArrays = (bgfx::getCaps()->supported & BGFX_CAPS_TEXTURE_2D_ARRAY) != 0u;
    if (!supportsTextureArrays) {
        numLayers = 1;
    }

    uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
    if (isPixelArt) {
        flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;
    }

    bgfx::TextureHandle handle; // NOLINT shut up it's initialized below
    auto createTexture = [&](uint32_t w, uint32_t h) {
        handle = bgfx::createTexture2D(
            w,
            h,
            false,
            numLayers,
            bgfx::TextureFormat::BC1,
            flags
        );

        if (!bgfx::isValid(handle)) {
            throw GmiException("Failed to load spritesheets: Unable to create texture");
        }

        m_handles.push_back(handle);
    };

    if (supportsTextureArrays) {
        createTexture(width, height);
    }

    uint8_t layer = 0;
    for (const auto& [filePath, sheet] : sheets) {
        if (!supportsTextureArrays) {
            createTexture(sheet.meta.size.w, sheet.meta.size.h);
        }

        for (const auto& [subName, frame] : sheet.frames) {
            m_textures[subName] = {
                .handle = handle,
                .layer = layer,
                .size = {
                    .w = sheet.meta.size.w,
                    .h = sheet.meta.size.h,
                },
                .frame = frame.frame,
                .scale = sheet.meta.scale,
            };
        }

        auto* t = new Timer("image parse");
        std::string sheetPath = (std::filesystem::path(filePath).parent_path() / sheet.meta.image).string();
        readImage(
            sheetPath,
            [handle, layer, t](const Image& image) {
                bgfx::updateTexture2D(
                    handle,
                    layer,
                    0,
                    0,
                    0,
                    image.width,
                    image.height,
                    image.data
                );
                t->stop();
            },
            "Failed to load spritesheet"
        );

        if (supportsTextureArrays) {
            ++layer;
        }
    }
}

Texture& TextureManager::get(const std::string& name) {
    if (!m_textures.contains(name)) {
        throw GmiException("Texture not found: '" + name + "'");
    }
    return m_textures[name];
}

void TextureManager::updateOnLoad(const std::string& name, Container* container) {
    m_pendingTextureUpdates[name].push_back(container);
}

void TextureManager::destroyAll() const {
    for (const bgfx::TextureHandle& handle : m_handles) {
        bgfx::destroy(handle);
    }
    if (bgfx::isValid(m_placeholderHandle)) {
        bgfx::destroy(m_placeholderHandle);
    }
}

void TextureManager::ensureTextureDoesNotExist(const std::string& name) const {
    if (m_textures.contains(name)) {
        throw GmiException("Failed to load texture '" + name + "': Texture already exists");
    }
}

TextureManager::TextureInfo TextureManager::createTexture(
    const Buffer& data,
    const std::string& name,
    bool isPixelArt
) {
    // auto [rawData, size, width, height] = imageParse(data);
    //
    // uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
    // if (isPixelArt) {
    //     flags |= BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;
    // }
    //
    // bgfx::TextureHandle handle = bgfx::createTexture2D(
    //     width,
    //     height,
    //     false,
    //     1u,
    //     bgfx::TextureFormat::RGBA8,
    //     flags,
    //     bgfx::copy(rawData, size)
    // );
    // free(rawData);
    //
    // if (!bgfx::isValid(handle)) {
    //     throw GmiException("Failed to load texture '" + name + "': Texture is not valid");
    // }
    //
    // m_handles.push_back(handle);
    //
    // return {handle, width, height};
}

void TextureManager::initPlaceholderHandle() {
    if (bgfx::isValid(m_placeholderHandle)) return;

    // Create a placeholder 1x1 texture if it doesn't exist
    // We have to do this here because bgfx isn't initialized yet in the constructor
    uint32_t black = 0xff000000;
    m_placeholderHandle = bgfx::createTexture2D(
        1,
        1,
        false,
        1u,
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE,
        bgfx::copy(&black, sizeof(black))
    );
}

void TextureManager::releaseTextureUpdates(const std::string& name) {
    for (Container* container : m_pendingTextureUpdates[name]) {
        container->setDirty();
    }
    m_pendingTextureUpdates.erase(name);
}

}
