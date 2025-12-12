#include "gmi/Application.h"

using namespace gmi;

Application* gmiMain() {
    auto* app = new Application({
        .width = 960,
        .height = 540,
        .title = "Hello World!",
        .backgroundColor = Color::fromRgb(128, 128, 255),
    });
    return app;
}
