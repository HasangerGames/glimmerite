#include <algorithm>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "resvg.h"
#include "gmi/util/util.h"

#include "rectpack2D/finders_interface.h"

#include "webp/encode.h"

#include "gmi/client/Affine.h"
#include "gmi/util/spritesheet.h"

using namespace gmi;
namespace fs = std::filesystem;

using SpacesType = rectpack2D::empty_spaces<false>;
using Rect = rectpack2D::output_rect_t<SpacesType>;

struct Sprite {
    std::string name;
    Rect rect;
    resvg_render_tree* tree;

    auto& get_rect() { return rect; }
    const auto& get_rect() const { return rect; }
};

struct Atlas {
    uint16_t w;
    uint16_t h;
    std::vector<Sprite> sprites;
    Spritesheet spritesheet;
};

int main(int argc, char* argv[]) {
    constexpr uint16_t maxAtlasSize = 4096;
    constexpr int discardStep       = 1;
    constexpr uint8_t padding       = 2;

    Timer total;
    Timer parseSprites;
    std::vector<Sprite> sprites;
    resvg_options* opt = resvg_options_create();
    size_t spriteMaxW = 0, spriteMaxH = 0;

    auto files = std::ifstream("files.txt");
    std::string filePath;
    while (std::getline(files, filePath)) {
        if (filePath.starts_with("#")) continue; // allow comments
        if (!filePath.ends_with(".svg")) continue; // ignore anything that isn't an SVG

        resvg_render_tree* tree;
        auto err = (resvg_error) resvg_parse_tree_from_file(filePath.c_str(), opt, &tree);
        if (err != RESVG_OK) {
            std::string errorMessage;
            switch (err) {
                case RESVG_ERROR_NOT_AN_UTF8_STR:
                    errorMessage = "File is not UTF-8";
                    break;
                case RESVG_ERROR_FILE_OPEN_FAILED:
                    errorMessage = "Failed to open file";
                    break;
                case RESVG_ERROR_MALFORMED_GZIP:
                    errorMessage = "Compressed SVG must use the GZip algorithm";
                    break;
                case RESVG_ERROR_ELEMENTS_LIMIT_REACHED:
                    errorMessage = "SVGs with more than 1,000,000 elements are unsupported";
                    break;
                case RESVG_ERROR_INVALID_SIZE:
                    errorMessage = "Invalid size. width, height and viewBox must be set";
                    break;
                case RESVG_ERROR_PARSING_FAILED:
                    errorMessage = "Failed to parse SVG data";
                    break;
                default:
                    break;
            }
            std::cerr << std::format("Failed to parse sprite '{}': {}\n", filePath, errorMessage);
            return 1;
        }

        auto [width, height] = resvg_get_image_size(tree);
        width += padding * 2;
        height += padding * 2;
        if (width > maxAtlasSize || height > maxAtlasSize) {
            std::cerr << std::format(
                "Sprite '{}' exceeds maximum atlas size: width + padding {}, height + padding {}, maximum {}\n",
                filePath, width, height, maxAtlasSize
            );
            return 1;
        }

        std::string name = std::filesystem::path(filePath).stem().string();
        sprites.emplace_back(name, rectpack2D::rect_xywh(-1, -1, (int) width, (int) height), tree);
        spriteMaxW = std::max((size_t) width, spriteMaxW);
        spriteMaxH = std::max((size_t) height, spriteMaxH);
    }

    // for (const std::string& path : paths) {
    //     for (const fs::directory_entry& entry : fs::recursive_directory_iterator(path)) {
    //         if (entry.is_directory()) continue;
    //     }
    // }

    parseSprites.stop(std::format("{} sprites parsed", sprites.size()));

    Timer packSprites;
    std::vector<Atlas> atlases;
    size_t atlasMaxW = 0, atlasMaxH = 0;
    constexpr auto insertCallback = [](auto&) {
        return rectpack2D::callback_result::CONTINUE_PACKING;
    };
    auto finderInput = rectpack2D::make_finder_input(
        maxAtlasSize,
        discardStep,
        insertCallback,
        insertCallback,
        rectpack2D::flipping_option::DISABLED
    );
    while (!sprites.empty()) {
        rectpack2D::rect_wh result = rectpack2D::find_best_packing<SpacesType>(sprites, finderInput);

        Atlas atlas = {
            .w = (uint16_t) result.w,
            .h = (uint16_t) result.h,
        };

        for (size_t i = 0; i < sprites.size(); ) {
            const Sprite& sprite = sprites[i];
            if (sprite.rect.x == -1) { // -1 means sprite hasn't been packed yet
                ++i;
                continue;
            }

            atlas.spritesheet.frames[sprite.name] = {
                .frame = {
                    .x = (uint16_t) sprite.rect.x,
                    .y = (uint16_t) sprite.rect.y,
                    .w = (uint16_t) sprite.rect.w,
                    .h = (uint16_t) sprite.rect.h
                }
            };

            // move sprite from sprites to packedSprites
            atlas.sprites.push_back(std::move(sprites[i]));
            sprites[i] = std::move(sprites.back());
            sprites.pop_back();
        }

        atlases.emplace_back(atlas);
        atlasMaxW = std::max((size_t) result.w, atlasMaxW);
        atlasMaxH = std::max((size_t) result.h, atlasMaxH);
    }
    packSprites.stop(std::format("{} atlases packed", atlases.size()));

    WebPConfig config;
    WebPConfigInit(&config);
    config.method = 6;
    config.lossless = 1;
    config.quality = 100;
    config.thread_level = 1;

    auto atlasBuffer = Buffer(atlasMaxW * atlasMaxH * 4);
    auto spriteBuffer = Buffer(spriteMaxW * spriteMaxH * 4);
    auto webpBuffer = Buffer();
    webpBuffer.reserve(6'000'000); // spritesheets will rarely exceed 6 MB

    WebPPicture picture;
    WebPPictureInit(&picture);
    picture.custom_ptr = &webpBuffer;
    picture.writer = [](const uint8_t* data, size_t size, const WebPPicture* picture) -> int {
        Buffer* buffer = static_cast<Buffer*>(picture->custom_ptr);
        size_t oldSize = buffer->size();
        buffer->resize(oldSize + size);
        std::memcpy(buffer->data() + oldSize, data, size);
        return 1;
    };

    for (size_t i = 0; i < atlases.size(); ++i) {
        Timer renderAtlas;
        const Atlas& atlas = atlases[i];

        // Zero out only the portion of the atlas buffer we need
        // Don't bother zeroing if we just allocated the buffer (i == 0)
        if (i != 0) std::memset(atlasBuffer.data(), 0, (size_t) atlas.w * atlas.h * 4);

        for (size_t j = 0; j < atlas.sprites.size(); ++j) {
            const Sprite& sprite = atlas.sprites[j];
            Rect rect = sprite.rect;

            // Zero out the sprite buffer only if necessary
            if (i != 0 || j != 0) std::memset(spriteBuffer.data(), 0, (size_t) rect.w * rect.h * 4);

            rect.x += padding;
            rect.y += padding;
            rect.w -= padding * 2;
            rect.h -= padding * 2;

            resvg_render(
                sprite.tree,
                resvg_transform_identity(),
                rect.w,
                rect.h,
                spriteBuffer.data()
            );

            // Copy the sprite data from the sprite buffer to the atlas buffer
            for (int y = 0; y < rect.h; ++y) {
                std::memcpy(
                    atlasBuffer.data() + (ptrdiff_t) ((rect.y + y) * atlas.w + rect.x) * 4,
                    spriteBuffer.data() + (ptrdiff_t) y * rect.w * 4,
                    (size_t) rect.w * 4
                );
            }
        }

        picture.width = atlas.w;
        picture.height = atlas.h;

        WebPPictureImportRGBA(
            &picture,
            reinterpret_cast<const uint8_t*>(atlasBuffer.data()),
            atlas.w * 4
        );

        webpBuffer.clear();

        WebPEncode(&config, &picture);

        std::string filename = std::format("output/atlas{}.webp", i + 1);
        auto output = std::ofstream(filename);
        if (!output.good()) {
            std::cerr << "Unable to write to file: " << filename << '\n';
            return 1;
        }
        output.write(webpBuffer.data(), webpBuffer.size());
        output.close();

        renderAtlas.stop(std::format("atlas {} ({} sprites) rendered", i + 1, atlas.sprites.size()));
    }
    WebPPictureFree(&picture);

    total.stop(std::format("{} atlases rendered", atlases.size()));
    return 0;
}
