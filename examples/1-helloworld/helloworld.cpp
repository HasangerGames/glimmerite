#include "gmi/Application.h"

using namespace gmi;

Application* gmiMain() {
    const auto app = new Application({
        .width = 960,
        .height = 540,
        .title = "Hello World!",
        .backgroundColor = Color::fromRgb(128, 128, 255),
        .backend = BackendType::Sdl
    });
    return app;
}
