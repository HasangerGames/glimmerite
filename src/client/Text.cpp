#include "gmi/client/Text.h"

#include <string>

namespace gmi {

class Container;
class Application;

Text::Text(Application* parentApp, Container* parent, const std::string& text) :
    Container(parentApp, parent), m_text(text) {

}

}
