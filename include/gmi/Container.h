#pragma once
#include <memory>
#include <vector>

#include "Backend.h"
#include "math/Affine.h"
#include "math/Transform.h"

namespace gmi {

class Container {
protected:
    Container* m_parent{nullptr};
    std::vector<std::unique_ptr<Container>> m_children;
    math::Transform m_transform;
    math::Affine m_affine;
    int m_zIndex{0};
public:
    Container() = default;
    explicit Container(const math::Transform& transform) : m_transform(transform) {
        m_affine = math::transformToAffine(m_transform);
    }
    virtual ~Container();

    /**
     * Adds a child to this Container. If the given Container has an existing parent,
     * it will be removed from that parent before it is added to this one.
     * @param child The child to add
     */
    Container* addChild(std::unique_ptr<Container> child);

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
     * Setting Z index of a child will have no effect unless this method is called.
     */
    void sortChildren();

    /** @return The Transform applied to this Container (position, rotation, scale, etc.) */
    [[nodiscard]] const math::Transform& getTransform() const { return m_transform; }

    virtual void setTransform(const math::Transform& transform) {
        m_transform = transform;
        m_affine = math::transformToAffine(m_transform);
    }

    /** @return This Container's Z index */
    [[nodiscard]] int getZIndex() const { return m_zIndex; }

    /**
     * Sets the Z index of this Container.
     * @param zIndex The new Z index to set
     */
    void setZIndex(const int zIndex) { m_zIndex = zIndex; }

    /** @return A pointer to this Container's parent. If this Container does not have a parent, returns nullptr. */
    [[nodiscard]] Container* getParent() const { return m_parent; }

    /**
     * Renders the contents of this Container using the given @ref Backend.
     * @param backend The backend to use
     */
    virtual void render(Backend& backend, const math::Affine& affine) const;
};

// this function must be here and not in Container.cpp or it'll cause linker errors
template<typename T, typename... Args>
T* Container::createChild(Args&&... args) {
    m_children.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    auto childPtr{static_cast<T*>(m_children.back().get())};
    childPtr->m_parent = this;
    return childPtr;
}

}
