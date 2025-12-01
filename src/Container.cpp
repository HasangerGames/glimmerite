#include "gmi/Container.h"

#include <algorithm>

#include "gmi/math/Affine.h"

namespace gmi {

Container::~Container() = default;

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

void Container::setPosition(const math::Vec2& position) {
    m_transform.position = position;
    updateAffine();
}

void Container::setRotation(const float rotation) {
    m_transform.rotation = rotation;
    updateAffine();
}

void Container::setScale(const math::Vec2& scale) {
    m_transform.scale = scale;
    updateAffine();
}

void Container::setScale(const float scale) {
    m_transform.scale = {scale, scale};
    updateAffine();
}

void Container::setPivot(const math::Vec2 &pivot) {
    m_transform.pivot = pivot;
    updateAffine();
}

void Container::updateAffine() {
    const math::Affine affine = math::Affine::fromTransform(m_transform);
    if (m_parent) {
        m_affine = m_parent->m_affine * affine;
    } else {
        m_affine = affine;
    }

    for (const auto& child : m_children) {
        child->updateAffine();
    }
}

void Container::animate(const AnimateOptions<math::Vec2>& opts) {

}


void Container::render(Backend &backend) const {
    for (const auto& child : m_children) {
        child->render(backend);
    }
}

}
