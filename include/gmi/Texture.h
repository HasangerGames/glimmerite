#pragma once

namespace gmi {

class Texture {
public:
    virtual ~Texture() = default;

    /** @return The width of this Texture in pixels */
    virtual int getWidth() = 0;

    /** @return The height of this Texture in pixels */
    virtual int getHeight() = 0;

    /** @return A pointer to the library-specific texture object backing this Texture */
    virtual void* getRawTexture() = 0;
};

}
