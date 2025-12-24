#include "gmi/client/Application.h"

using namespace gmi;

void gmiMain(Application& app) {
    app.init({
        .width = 720,
        .height = 540,
        .title = "Hello World!",
        .backgroundColor = Color::rgb(128, 128, 255),
    });
}
