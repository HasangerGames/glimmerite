# Glimmerite
An experimental 2D renderer/game engine for C++, inspired by [Pixi.js](https://github.com/pixijs/pixijs). Built with [SDL3](https://github.com/libsdl-org/SDL) and [bgfx](https://github.com/bkaradzic/bgfx).

## Features
- **Performance:** Glimmerite is designed to be extremely fast for rendering sprites and basic 2D geometry (rectangles, circles, polygons).
- **Compatibility:** Apps built with Glimmerite work on Windows, macOS, Linux, iOS, Android, and the browser through WebAssembly. All major graphics APIs are supported through bgfx, including Vulkan, Metal, Direct3D 12 and 11, OpenGL, and OpenGL ES.

## Prerequisites
- C++23 or later
- CMake 4.0 or later

## Quick start
```cpp
#include "gmi/Application.h"

using namespace gmi;

Application* gmiMain() {
    const auto app = new Application({
        .width = 960,
        .height = 540,
        .title = "Hello World!",
        .backgroundColor = Color::fromRgb(128, 128, 255),
    });
    return app;
}
```
See [the examples](https://github.com/HasangerGames/glimmerite/tree/main/examples/) for more.

## Projects
- Suroi: https://github.com/HasangerGames/suroi-client

Built something cool with Glimmerite and want it featured here? Please [submit a pull request](https://github.com/HasangerGames/glimmerite/pulls)!
