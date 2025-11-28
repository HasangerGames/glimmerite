#pragma once
#include <memory>
#include <vector>

#include "Backend.h"
#include "math/Transform.h"

namespace gmi {

class Container {
protected:
    Container* m_parent = nullptr;
    std::vector<std::unique_ptr<Container>> m_children;
    math::Transform m_transform;
    int m_zIndex = 0;
    bool m_autoSortChildren = false;
public:
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
     * Manually sorts this Container's children by Z index.
     * If auto sorting is enabled using `setAutoSortChildren(true)`,
     * this method will be called automatically when adding a child or changing its Z index.
     */
    void sortChildren();

    /** @return Whether sortChildren() should be called automatically when adding a child or changing its Z index. */
    [[nodiscard]] bool getAutoSortChildren() const { return m_autoSortChildren; }

    /**
     * Sets whether sortChildren() should be called automatically when adding a child or changing its Z index.
     * @param autoSortChildren Whether to sort children automatically
     */
    void setAutoSortChildren(const bool autoSortChildren) { m_autoSortChildren = autoSortChildren; }

    [[nodiscard]] math::Transform& getTransform() { return m_transform; }

    /** @return This Container's Z index */
    [[nodiscard]] int getZIndex() const { return m_zIndex; }

    /**
     * Sets the Z index of this Container.
     * @param zIndex The new Z index to set
     */
    void setZIndex(int zIndex);

    /** @return A pointer to this Container's parent. If this Container does not have a parent, returns nullptr. */
    [[nodiscard]] Container* getParent() const { return m_parent; }

    /**
     * Renders the contents of this Container using the given @ref Backend, applying the given @ref math::Transform.
     * @param backend The backend to use
     * @param transform The transform to apply
     */
    virtual void render(Backend& backend, const math::Transform& transform) const;
};

// this function must be here and not in Container.cpp or it'll cause linker errors
template<typename T, typename... Args>
T* Container::createChild(Args&&... args) {
    m_children.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    if (m_autoSortChildren) sortChildren();

    T* childPtr = static_cast<T*>(m_children.back().get());
    childPtr->m_parent = this;
    return childPtr;
}

}
