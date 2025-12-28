#include "gmi/client/Application.h"
#include "gmi/client/Sprite.h"

using namespace gmi;

void gmiMain(Application& app) {
    app.init({
        .title = "Glimmerite Sprites Demo",
        .backgroundColor = Color::rgb(128, 128, 255),
    });

    app.textures().load("assets/bunny.png", true);

    auto& bunny = app.stage().createChild<Sprite>("bunny", math::Transform{
        .position = {400, 300},
        .scale = {4, 4},
    });
    bunny.animate(AnimateOptions{
        .prop = math::TransformProps::Rotation,
        .target = math::PI * 2,
        .duration = 1500,
        .infinite = true
    });
}
