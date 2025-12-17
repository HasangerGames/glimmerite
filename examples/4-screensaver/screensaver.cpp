#include "gmi/Application.h"
#include "gmi/Sprite.h"
#include "gmi/math/random.h"

using namespace gmi;

float SPEED = 0.5f;
math::Vec2 VELOCITY = {SPEED, SPEED};

void gmiMain(Application& app) {
    app.init({
        .title = "Glimmerite DVD Screensaver",
        .backgroundColor = Color::Black,
    });

    app.textures().load("assets/dvd_logo.png");
    auto [logoWidth, logoHeight] = app.textures().get("dvd_logo").size;
    auto xMax = static_cast<float>(800 - logoWidth);
    auto yMax = static_cast<float>(600 - logoHeight);

    auto& dvdLogo = app.stage().createChild<Sprite>("dvd_logo");
    dvdLogo.setPosition(math::Vec2::fromAny(math::random(0, static_cast<int>(xMax)), math::random(0, static_cast<int>(yMax))));
    dvdLogo.setPivot({0, 0}); // Makes the math simpler
    app.addTicker([&app, &dvdLogo, xMax, yMax] {
        math::Vec2 position = dvdLogo.getPosition();
        bool xFlip = position.x < 0 || position.x > xMax;
        bool yFlip = position.y < 0 || position.y > yMax;
        if (xFlip) VELOCITY.x *= -1;
        if (yFlip) VELOCITY.y *= -1;
        if (xFlip || yFlip) {
            dvdLogo.setTint(Color::rgb(math::random(128, 255), math::random(128, 255), math::random(128, 255)));
        }
        dvdLogo.setPosition(position + VELOCITY * app.getDt());
    });
}
