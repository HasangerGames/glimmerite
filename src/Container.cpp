#include "gmi/Container.h"

#include <algorithm>

#include "gmi/gmi.h"

namespace gmi {

Container::~Container() = default;

Container* Container::addChild(std::unique_ptr<Container> child) {
    if (child.get() == this) {
        throw GmiException("Container cannot be its own child");
    }

    if (child->m_parent) {
        child->m_parent->removeChild(child.get());
    }
    child->m_parent = this;

    m_children.push_back(std::move(child));
    if (m_autoSortChildren) sortChildren();
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

void Container::render(Backend &backend, const math::Transform& transform) const {
    for (const auto& child : m_children) {
        child->render(backend, math::combineTransforms(transform, m_transform));
    }
}

}
