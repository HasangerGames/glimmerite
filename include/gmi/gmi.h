#pragma once

#include <vector>

#include "Application.h"

namespace gmi {

std::vector<GraphicsApi> getSupportedGraphicsApis(RendererType rendererType = RendererType::Auto);

}
