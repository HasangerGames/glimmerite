#pragma once

#include "Drawable.h"
#include "gmi/client/Container.h"

namespace gmi {

class Text : public Container {
    std::string m_text;
    Drawable m_drawable;

public:
    Text(Application* parentApp, Container* parent, const std::string& text);

    void setText(const std::string& text) { m_text = text; }

    void render(Renderer& renderer) override;
};

}
