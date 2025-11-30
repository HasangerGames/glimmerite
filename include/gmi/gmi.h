#pragma once

#include <set>

namespace gmi {

struct GmiException final : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * Checks which renderers are supported with the given backend.
 * @param backend The @ref BackendType to use to check supported renderers.
 *                Different backends may support different sets of renderers.
 *                Currently defaults to bgfx.
 * @return
 */
[[nodiscard]] std::set<RendererType> getSupportedRenderers(BackendType backend);

[[nodiscard]] std::string getRendererName(RendererType rendererType);

}
