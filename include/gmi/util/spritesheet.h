#pragma once

#include <string>
#include <unordered_map>

#include "gmi/math/Rect.h"
#include "gmi/math/Size.h"

namespace gmi {

struct SpritesheetRect {
    uint16_t x, y, w, h;
};

struct SpritesheetSize {
    uint16_t w, h;
};

struct SpritesheetFrame {
    SpritesheetRect frame;
    SpritesheetSize sourceSize;
};

struct SpritesheetMeta {
    std::string image;
    float scale;
    SpritesheetSize size;
};

using SpritesheetFrames = std::unordered_map<std::string, SpritesheetFrame>;

struct Spritesheet {
    SpritesheetMeta meta;
    SpritesheetFrames frames;
};

}
