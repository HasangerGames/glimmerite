#include "gmi/Container.h"

#include <algorithm>

#include "gmi/gmi.h"
#include "gmi/math/Affine.h"

namespace gmi {

Container::~Container() = default;

Container* Container::addChild(std::unique_ptr<Container> child) {
    Container* childPtr = child.get();
    if (childPtr == this) {
        throw GmiException("Container cannot be its own child");
    }

    if (child->m_parent) {
        child->m_parent->removeChild(childPtr);
    }
    child->m_parent = this;

    m_children.push_back(std::move(child));
    return m_children.back().get();
}

void Container::removeChild(Container* child) {
    const auto it{std::ranges::find_if(
        m_children,
        [child](const std::unique_ptr<Container>& m_child) { return m_child.get() == child; }
    )};
    if (it != m_children.end()) {
        m_children.erase(it);
        child->m_parent = nullptr;
    }
}

void Container::sortChildren() {
    std::ranges::sort(
        m_children,
        [](const std::unique_ptr<Container>& a, const std::unique_ptr<Container>& b) { return a->m_zIndex < b->m_zIndex; }
    );
}

void Container::render(Backend &backend, const math::Affine& affine = {}) const {
    for (const auto& child : m_children) {
        child->render(backend, m_affine * affine);
    }
}

}
