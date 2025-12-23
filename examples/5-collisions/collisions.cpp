#include "SDL3/SDL_events.h"
#include "gmi/client/Application.h"
#include "gmi/client/Graphics.h"
#include "gmi/math/Shape.h"
#include "gmi/math/collision.h"

#include <memory>

using namespace gmi;
using namespace gmi::math;

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
constexpr int BORDER_SIZE = 20;

struct DemoShape {
    std::unique_ptr<collision::Shape> shape;

    Color color;
    bool isStatic;
};

std::vector<DemoShape> SHAPES;
int SELECTED_SHAPE = -1;

void checkCollision(DemoShape& shape) {
    bool collided = true;

    // yes this is "slow"
    // no i don't care, there's not that many shapes in this demo
    // doing 20 collision steps to make sure it separates properly more often
    for (size_t i = 0; i < 20 && collided; i++) {
        collided = false;
        for (const auto& shapeB : SHAPES) {
            if (&shapeB == &shape) continue;

            collision::Response res;
            if (shape.shape->getCollision(*shapeB.shape, &res)) {
                shape.shape->translate(res.normal * (-res.depth));
                collided = true;
            }
        }
    }
};

void gmiMain(Application& app) {
    app.init({
        .width = WIDTH,
        .height = HEIGHT,
        .title = "Shapes!",
        .backgroundColor = Color::rgb(100, 100, 100),
        .resizable = false,
    });

    auto& gfx = app.stage().createChild<Graphics>();

    SHAPES.reserve(10);

    // top border
    SHAPES.emplace_back(
        std::make_unique<collision::Rect>(Vec2f(0, 0), Vec2f(WIDTH, BORDER_SIZE)),
        Color::rgb(0, 0, 0),
        true
    );

    // left border
    SHAPES.emplace_back(
        std::make_unique<collision::Rect>(Vec2f(0, 0), Vec2f(BORDER_SIZE, HEIGHT)),
        Color::rgb(0, 0, 0),
        true
    );

    // bottom border
    SHAPES.emplace_back(
        std::make_unique<collision::Rect>(Vec2f(0, HEIGHT - BORDER_SIZE), Vec2f(WIDTH, HEIGHT)),
        Color::rgb(0, 0, 0),
        true
    );

    // right border
    SHAPES.emplace_back(
        std::make_unique<collision::Rect>(Vec2f(WIDTH - BORDER_SIZE, 0), Vec2f(HEIGHT, WIDTH)),
        Color::rgb(0, 0, 0),
        true
    );

    //
    // Random shapes
    //

    SHAPES.emplace_back(
        std::make_unique<collision::Circle>(Vec2f{90.0f, 100.0f}, 30),
        Color::Blue,
        false
    );

    SHAPES.emplace_back(
        std::make_unique<collision::Circle>(Vec2f{100.0f, 400.0f}, 60),
        Color::Green,
        false
    );

    SHAPES.emplace_back(
        std::make_unique<collision::Rect>(collision::Rect::fromDims(80, 100, Vec2f{600, 600})),
        Color::Cyan,
        false
    );

    SHAPES.emplace_back(
        std::make_unique<collision::Polygon>(collision::Polygon::fromSides(6, {400, 400}, 80)),
        Color::Red,
        false
    );

    SHAPES.emplace_back(
        std::make_unique<collision::Polygon>(collision::Polygon::fromSides(3, {600, 300}, 60)),
        Color::Yellow,
        false
    );

    SHAPES.emplace_back(
        std::make_unique<collision::Rect>(collision::Rect::fromDims(80, 20, Vec2f{100, 600})),
        Color::Yellow,
        false
    );

    app.addTicker([&app, &gfx]() {
        gfx.clear();
        for (const auto& shape : SHAPES) {
            gfx.fillShape(*shape.shape, shape.color);
        }
    });

    // select / deselect / move selected shape
    app.addEventListener(SDL_EVENT_MOUSE_BUTTON_DOWN, [](const SDL_Event& event) {
        if (event.button.button != 1) return;

        Vec2f pos{event.button.x, event.button.y};
        for (size_t i = 0; i < SHAPES.size(); i++) {
            auto& shape = SHAPES[i];
            if (shape.isStatic) continue;

            if (shape.shape->pointInside(pos)) {
                SELECTED_SHAPE = i;
                break;
            }
        }
    });

    app.addEventListener(SDL_EVENT_MOUSE_BUTTON_UP, [](const SDL_Event& event) {
        if (event.button.button != 1) return;

        SELECTED_SHAPE = -1;
    });

    app.addEventListener(SDL_EVENT_MOUSE_MOTION, [](const SDL_Event& event) {
        if (SELECTED_SHAPE < 0) return;
        auto& shape = SHAPES[SELECTED_SHAPE];

        shape.shape->translate({event.motion.xrel, event.motion.yrel});

        checkCollision(shape);
    });

    // scale shapes with mouse wheel
    app.addEventListener(SDL_EVENT_MOUSE_WHEEL, [](const SDL_Event& event) {
        Vec2f pos{event.wheel.mouse_x, event.wheel.mouse_y};

        float scale = event.wheel.y < 0 ? 0.95 : 1.05;

        for (auto& shape : SHAPES) {
            if (shape.isStatic) continue;

            if (shape.shape->pointInside(pos)) {
                shape.shape->scale(scale);
                checkCollision(shape);
                break;
            }
        }
    });
}
