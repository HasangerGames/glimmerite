#include "SdlTexture.h"

namespace gmi {

int SdlTexture::getWidth() {
    return m_rawTexture->w;
}

int SdlTexture::getHeight() {
    return m_rawTexture->h;
}

}
