#pragma once

#include <vector>

#include "Application.h"

namespace gmi {

std::vector<GraphicsApi> getSupportedRenderers(BackendType rendererType = BackendType::Auto);

}
