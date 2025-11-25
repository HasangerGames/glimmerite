#pragma once
#include "Vec2.h"

namespace gmi::math {

struct Transform {
    Vec2 position;
    float rotation;
    Vec2 scale;
};

}
