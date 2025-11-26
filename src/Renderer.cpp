#include "gmi/Application.h"

namespace gmi {

Backend::Backend(Application *parentApp) : m_parentApp(parentApp) {}

std::unique_ptr<Backend> createBackend(Application *parentApp, GraphicsApi graphicsApi) {

}

}
