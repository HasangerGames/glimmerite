#include "gmi/Container.h"

#include <algorithm>

#include "gmi/Application.h"
#include "gmi/gmi.h"
#include "gmi/math/Affine.h"

namespace gmi {

Container::~Container() = default;

void Container::removeChild(Container* child) {
    auto it = std::ranges::find_if(
        m_children,
        [child](const std::unique_ptr<Container>& m_child) {
            return m_child.get() == child;
        }
    );
    if (it != m_children.end()) {
        m_children.erase(it);
        child->m_parent = nullptr;
    }
}

void Container::sortChildren() {
    std::ranges::sort(
        m_children,
        [](const std::unique_ptr<Container>& a, const std::unique_ptr<Container>& b) {
            return a->m_zIndex < b->m_zIndex;
        }
    );
}

void Container::setPosition(const math::Vec2& position) {
    m_transform.position = position;
    m_transformDirty = true;
}

void Container::setRotation(float rotation) {
    m_transform.rotation = rotation;
    m_transformDirty = true;
}

void Container::setScale(const math::Vec2& scale) {
    m_transform.scale = scale;
    m_transformDirty = true;
}

void Container::setScale(float scale) {
    m_transform.scale = {scale, scale};
    m_transformDirty = true;
}

void Container::setPivot(const math::Vec2& pivot) {
    m_transform.pivot = pivot;
    m_transformDirty = true;
}

void Container::updateAffine() {
    const math::Affine affine = math::Affine::fromTransform(m_transform);
    if (m_parent != nullptr) {
        m_affine = m_parent->m_affine * affine;
    } else {
        m_affine = affine;
    }

    for (const auto& child : m_children) {
        child->updateAffine();
    }
}

void Container::animate(const AnimateOptions<math::Vec2>& opts) {
    math::Vec2* prop;
    switch (opts.prop) {
    case math::TransformProps::Position:
        prop = &m_transform.position;
        break;
    case math::TransformProps::Scale:
        prop = &m_transform.scale;
        break;
    case math::TransformProps::Pivot:
        prop = &m_transform.pivot;
        break;
    default:
        throw GmiException("Attempted to animate a non-Vec2 property to a Vec2 target");
    }
    m_parentApp->tween().add({
        .values = {{&prop->x, opts.target.x}, {&prop->y, opts.target.y}},
        .duration = opts.duration,
        .ease = math::Easing::cubicOut,
        .yoyo = true,
        .onUpdate = [this] {
            m_transformDirty = true;
        },
    });
}

void Container::render(Renderer& renderer) {
    if (m_transformDirty) {
        updateAffine();
    }

    for (const auto& child : m_children) {
        child->render(renderer);
    }
}

}
