#include "gmi/Application.h"
#include "gmi/Sprite.h"

using namespace gmi;

void gmiMain(Application& app) {
    app.init({
        .title = "Glimmerite Sprites Demo",
        .backgroundColor = Color::rgb(128, 128, 255),
    });

    app.textures().load("assets/bunny.png");

    auto& bunny = app.stage().createChild<Sprite>("bunny");
    bunny.setPosition({400, 300});
    bunny.setScale(4);
    bunny.animate(AnimateOptions{
        .prop = math::TransformProps::Rotation,
        .target = math::PI * 2,
        .duration = 1500,
        .infinite = true
    });
}
