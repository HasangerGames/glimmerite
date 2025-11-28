#include "gmi/Application.h"

int main() {
    gmi::Application app;
    app.init({
        .width = 960,
        .height = 540,
        .title = "Hello World!",
        .backgroundColor = gmi::Color::fromRgb(128, 128, 255)
    });

    app.start();
};
