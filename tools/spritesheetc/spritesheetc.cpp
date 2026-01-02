#include <algorithm>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "resvg.h"
#include "gmi/util/util.h"

#include "rectpack2D/finders_interface.h"

#include "fpng.h"
#include "gmi/client/Affine.h"

using namespace gmi;
namespace fs = std::filesystem;

using SpacesType = rectpack2D::empty_spaces<false>;
using Rect = rectpack2D::output_rect_t<SpacesType>;

struct Sprite {
    Rect rect;
    resvg_render_tree* tree;

    auto& get_rect() { return rect; }
    const auto& get_rect() const { return rect; }
};

struct Atlas {
    uint16_t w;
    uint16_t h;
    std::vector<Sprite> sprites;
};

int main(int argc, char* argv[]) {
    constexpr int maxBinSide  = 4096;
    constexpr int discardStep = 1;

    Timer total;
    Timer parseSprites;
    std::vector<Sprite> sprites;
    resvg_options* opt = resvg_options_create();
    size_t spriteMaxW = 0, spriteMaxH = 0;

    auto files = std::ifstream("files.txt");
    std::string filePath;
    while (std::getline(files, filePath)) {
        resvg_render_tree* tree;
        resvg_parse_tree_from_file(filePath.c_str(), opt, &tree);
        auto [width, height] = resvg_get_image_size(tree);
        sprites.emplace_back(rectpack2D::rect_xywh(-1, -1, (int) width, (int) height), tree);
        spriteMaxW = std::max((size_t) width, spriteMaxW);
        spriteMaxH = std::max((size_t) height, spriteMaxH);
    }
    // std::vector<std::string> paths = {
    //     "../../../Suroi/client/public/img/game/shared",
    //     "../../../Suroi/client/public/img/game/normal"
    // };
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
        maxBinSide,
        discardStep,
        insertCallback,
        insertCallback,
        rectpack2D::flipping_option::DISABLED
    );
    while (!sprites.empty()) {
        std::vector<Sprite> packedSprites;

        rectpack2D::rect_wh result = rectpack2D::find_best_packing<SpacesType>(sprites, finderInput);

        for (size_t i = 0; i < sprites.size(); ) {
            if (sprites[i].rect.x != -1) { // changed from -1 means sprite has been packed
                // move sprite from sprites to packedSprites
                packedSprites.push_back(sprites[i]);
                sprites[i] = sprites.back();
                sprites.pop_back();
            } else {
                ++i;
            }
        }

        atlases.emplace_back(result.w, result.h, packedSprites);
        atlasMaxW = std::max((size_t) result.w, atlasMaxW);
        atlasMaxH = std::max((size_t) result.h, atlasMaxH);
    }

    packSprites.stop(std::format("{} atlases packed", atlases.size()));

    fpng::fpng_init();

    auto atlasBuffer = Buffer(atlasMaxW * atlasMaxH * 4);
    auto spriteBuffer = Buffer(spriteMaxW * spriteMaxH * 4);
    auto pngBuffer = Buffer(6'000'000); // spritesheets will rarely exceed 6 MB
    for (size_t i = 0; i < atlases.size(); i++) {
        Timer renderAtlas;
        const Atlas& atlas = atlases[i];

        // zero out only the portion of the buffer we need
        // don't bother zeroing if we just allocated the buffer (i == 0)
        if (i != 0) std::memset(atlasBuffer.data(), 0, (size_t) atlas.w * atlas.h * 4);

        for (size_t j = 0; j < atlas.sprites.size(); ++j) {
            const auto& [rect, tree] = atlas.sprites[j];
            if (j != 0) std::memset(spriteBuffer.data(), 0, (size_t) rect.w * rect.h * 4);
            resvg_render(
                tree,
                resvg_transform_identity(),
                rect.w,
                rect.h,
                spriteBuffer.data()
            );
            for (int y = 0; y < rect.h; ++y) {
                std::memcpy(
                    atlasBuffer.data() + (ptrdiff_t) ((rect.y + y) * atlas.w + rect.x) * 4,
                    spriteBuffer.data() + (ptrdiff_t) y * rect.w * 4,
                    (size_t) rect.w * 4
                );
            }
        }

        fpng::fpng_encode_image_to_memory(
            atlasBuffer.data(),
            atlas.w,
            atlas.h,
            4,
            reinterpret_cast<std::vector<uint8_t>&>(pngBuffer)
        );

        auto output = std::ofstream(std::format("output/atlas{}.png", i + 1));
        output.write(pngBuffer.data(), (std::streamsize) pngBuffer.size());
        output.close();
        renderAtlas.stop(std::format("atlas {} ({} sprites) rendered", i + 1, atlas.sprites.size()));
    }
    return 0;
}
