#include <algorithm>

#include "gmi/client/Affine.h"
#include "gmi/client/Application.h"
#include "gmi/client/Container.h"
#include "gmi/client/gmi.h"

namespace gmi {

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

void Container::setPosition(math::Vec2f position) {
    m_transform.position = position;
    m_transformDirty = true;
}

void Container::setRotation(float rotation) {
    m_transform.rotation = rotation;
    m_transformDirty = true;
}

void Container::setScale(math::Vec2f scale) {
    m_transform.scale = scale;
    m_transformDirty = true;
}

void Container::setScale(float scale) {
    m_transform.scale = {scale, scale};
    m_transformDirty = true;
}

void Container::setTint(Color tint) {
    m_transform.color = tint;
    m_transformDirty = true;
}

void Container::setPivot(math::Vec2f pivot) {
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

void Container::animate(const AnimateOptions<math::Vec2f>& opts) {
    math::Vec2f* prop;
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
        throw GmiException("Attempted to animate a non-Vec2f property to a Vec2f target");
    }
    uint16_t tweenId = m_parentApp->tweens().add({
        .values = {{&prop->x, opts.target.x}, {&prop->y, opts.target.y}},
        .duration = opts.duration,
        .ease = opts.easing,
        .yoyo = opts.yoyo,
        .infinite = opts.infinite,
        .onComplete = [this, &tweenId] { removeAnim(tweenId); },
    });
    m_animations.emplace(opts.prop, tweenId);
}

void Container::animate(const AnimateOptions<float>& opts) {
    float* prop;
    switch (opts.prop) {
    case math::TransformProps::Rotation:
        prop = &m_transform.rotation;
        break;
    default:
        throw GmiException("Attempted to animate a non-float property to a float target");
    }
    uint16_t tweenId = m_parentApp->tweens().add({
        .values = {{prop, opts.target}},
        .duration = opts.duration,
        .ease = opts.easing,
        .yoyo = opts.yoyo,
        .infinite = opts.infinite,
        .onComplete = [this, &tweenId] { removeAnim(tweenId); },
    });
    m_animations.emplace(opts.prop, tweenId);
}

void Container::stopAnimate(math::TransformProps prop) {
    uint16_t id = m_animations[prop];
    m_parentApp->tweens().kill(id);
    removeAnim(id);
}

void Container::removeAnim(uint16_t id) {
    std::erase_if(m_animations, [id](const std::pair<math::TransformProps, uint16_t>& anim) { return anim.second == id; });
}

void Container::render(Renderer& renderer) {
    if (m_transformDirty || !m_animations.empty()) {
        updateAffine();
        m_transformDirty = false;
    }

    for (const auto& child : m_children) {
        child->render(renderer);
    }
}

}
