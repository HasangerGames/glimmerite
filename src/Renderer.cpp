#include "gmi/Application.h"

namespace gmi {

Renderer::Renderer(Application *parentApp) : m_parentApp(parentApp) {}

std::unique_ptr<Renderer> createRenderer(Application *parentApp, GraphicsApi graphicsApi) {

}

}
