#include "gmi/Application.h"

using namespace gmi;

Application* gmiMain() {
    auto* const app = new Application({
        .width = 960,
        .height = 540,
        .title = "Hello World!",
        .backgroundColor = Color::fromRgb(128, 128, 255),
    });
    return app;
}
