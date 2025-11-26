#pragma once
#include "util/Color.h"

namespace gmi {

class Application;
enum class BackendType;

/**
 * The Backend is a translation layer which takes simple drawing commands
 * such as drawRect, drawPolygon, and drawSprite, and
 */
class Backend {
    Application* m_parentApp;
public:
    explicit Backend(Application* parentApp);
    virtual ~Backend() = default;

    virtual BackendType getType();
    virtual RendererType getRenderer();

    virtual void init() = 0;

    virtual void renderFrame() = 0;
    virtual void setClearColor(Color color) = 0;
};

}
