#pragma once
#include "gmi/Application.h"

namespace gmi::internal {

RendererType sdlRendererNameToType(const std::string& name);

std::unique_ptr<Backend> createBackend(Application& parentApp, RendererType rendererType, BackendType backendType);

}
