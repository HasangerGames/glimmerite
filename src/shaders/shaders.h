#pragma once

#include "bgfx/embedded_shader.h"

#include "glsl/vs_sprite.sc.bin.h"
#include "essl/vs_sprite.sc.bin.h"
#include "spirv/vs_sprite.sc.bin.h"
#include "glsl/fs_sprite.sc.bin.h"
#include "essl/fs_sprite.sc.bin.h"
#include "spirv/fs_sprite.sc.bin.h"
#if defined(_WIN32)
#include "dx11/vs_sprite.sc.bin.h"
#include "dx11/fs_sprite.sc.bin.h"
#else
// makes bgfx embedded shader macro work if dx11 shaders aren't present
static constexpr uint8_t fs_sprite_dx11[0] = {};
static constexpr uint8_t vs_sprite_dx11[0] = {};
#endif //  defined(_WIN32)
#if __APPLE__
#include "mtl/vs_sprite.sc.bin.h"
#include "mtl/fs_sprite.sc.bin.h"
#endif // __APPLE__

namespace gmi::internal {

const bgfx::EmbeddedShader FS_SPRITE = BGFX_EMBEDDED_SHADER(fs_sprite);
const bgfx::EmbeddedShader VS_SPRITE = BGFX_EMBEDDED_SHADER(vs_sprite);

}
