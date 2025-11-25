#include "gmi/Application.h"

int main() {
    gmi::Application app;
    app.init({
        .width = 800,
        .height = 600,
        .title = "Hello World!"
    });

    while (!app.closeRequested()) {
        app.pollEvents();
    }

    app.shutdown();
};
