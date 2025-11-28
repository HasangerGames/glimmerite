#pragma once
#include "gmi/Texture.h"
#include "SDL3/SDL_render.h"

namespace gmi {

class SdlTexture final : public Texture {
    SDL_Texture* m_rawTexture;
public:
    explicit SdlTexture(SDL_Texture* texture) : m_rawTexture(texture) {}

    int getWidth() override;
    int getHeight() override;

    void* getRawTexture() override { return m_rawTexture; }
};

}
