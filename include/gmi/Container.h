#pragma once
#include <memory>
#include <vector>

#include "Backend.h"
#include "gmi.h"
#include "math/Affine.h"
#include "math/Easing.h"
#include "math/Transform.h"

namespace gmi {

template<typename T>
struct AnimateOptions {
    math::TransformProps prop;
    T target;
    uint64_t duration;
    math::EasingFn easing{math::Easing::linear};
    bool yoyo{false};
    bool infinite{false};
};

class Container {
protected:
    Container* m_parent{nullptr};
    std::vector<std::unique_ptr<Container>> m_children;

    math::Affine m_affine;
    int m_zIndex{0};
public:
    math::Transform m_transform;
    bool m_transformDirty{true};
    Container() = default;
    explicit Container(const math::Transform& transform) : m_transform(transform) {}
    virtual ~Container();

    /**
     * Adds a child to this Container. If the child has an existing parent,
     * it will be removed from that parent before it is added to this one.
     * @param child The child to add
     */
    template<typename T>
    T* addChild(std::unique_ptr<T> child);

    /**
     * Creates a child and adds it to this Container.
     * @tparam T The type of Container to create
     * @tparam Args The Container's constructor arguments
     * @param args The arguments to pass to the Container's constructor
     * @return
     */
    template<typename T, typename... Args>
    T* createChild(Args&&... args);

    /**
     * Removes a child from this Container.
     * @param child The child to remove
     */
    void removeChild(Container* child);

    /**
     * Sorts this Container's children by Z index.
     * Setting Z index of a child has no effect until this method is called.
     */
    void sortChildren();

    /** @param position The new position to set */
    void setPosition(const math::Vec2& position);

    /** @param rotation The new rotation to set */
    void setRotation(float rotation);

    /** @param scale The new scale to set */
    void setScale(const math::Vec2& scale);

    /** @param scale The new scale to set. Will be used for both x- and y-components. */
    void setScale(float scale);

    /** @param pivot The center of rotation to set, as a normalized vector (components 0-1) */
    void setPivot(const math::Vec2& pivot);

    /** @return The Transform applied to this Container (position, rotation, scale, etc.) */
    [[nodiscard]] const math::Transform& getTransform() const { return m_transform; }

    /**
     * Updates the internal matrix used to calculate transforms.
     * This method should never need to be called manually.
     */
    virtual void updateAffine();

    /** @return This Container's Z index */
    [[nodiscard]] int getZIndex() const { return m_zIndex; }

    /**
     * Sets the Z index of this Container.
     * @param zIndex The new Z index to set
     */
    void setZIndex(const int zIndex) { m_zIndex = zIndex; }

    void animate(const AnimateOptions<math::Vec2>& opts);

    /** @return A pointer to this Container's parent. If this Container does not have a parent, returns nullptr. */
    [[nodiscard]] Container* getParent() const { return m_parent; }

    /**
     * Renders the contents of this Container using the given @ref Backend.
     * @param backend The backend to use
     */
    virtual void render(Backend& backend);
};

template<typename T>
T* Container::addChild(std::unique_ptr<T> child) {
    Container* childPtr = child.get();
    if (childPtr == this) {
        throw GmiException("Container cannot be its own child");
    }

    if (child->m_parent) {
        child->m_parent->removeChild(childPtr);
    }
    child->m_parent = this;
    child->m_transformDirty = true;

    m_children.push_back(std::move(child));
    return m_children.back().get();
}

template<typename T, typename... Args>
T* Container::createChild(Args&&... args) {
    m_children.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    auto childPtr{static_cast<T*>(m_children.back().get())};
    childPtr->m_parent = this;
    return childPtr;
}

}
