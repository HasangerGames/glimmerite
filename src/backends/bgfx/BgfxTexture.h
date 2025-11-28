#pragma once
#include "gmi/Texture.h"

namespace gmi {

class BgfxTexture : public Texture {
public:
    void* getRawTexture() override;
};

}
