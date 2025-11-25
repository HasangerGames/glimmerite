#pragma once
#include "util/Color.h"

namespace gmi {

class Application;
enum class RendererType;

/**
 * The Renderer is a translation layer which takes simple drawing commands
 * such as drawRect, drawPolygon, drawSprite
 */
class Renderer {
    Application* m_parentApp;
public:
    explicit Renderer(Application* parentApp);
    virtual ~Renderer() = default;

    virtual RendererType getType();

    virtual void init() = 0;

    virtual void renderFrame() = 0;
    virtual void setClearColor(Color color) = 0;
};

}
