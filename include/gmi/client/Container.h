#pragma once
#include <memory>
#include <vector>

#include "gmi/client/Renderer.h"

#include "gmi/client/Affine.h"
#include "gmi/math/Easing.h"

namespace gmi {

template<typename T>
struct AnimateOptions {
    math::TransformProps prop;
    T target;
    uint64_t duration;
    math::EasingFn easing = math::Easing::linear;
    bool yoyo = false;
    bool infinite = false;
};

class Application;
class Renderer;

class Container {
protected:
    Application* m_parentApp = nullptr;
    Container* m_parent = nullptr;
    std::vector<std::unique_ptr<Container>> m_children;

    math::Affine m_affine;
    math::Transform m_transform;
    bool m_transformDirty = true;

    int m_zIndex = 0;
    bool m_visible = true;

    virtual void updateAffine();

public:
    Container() = default;

    Container(Application* parentApp, Container* parent) :
        m_parentApp(parentApp), m_parent(parent) { }

    Container(Application* parentApp, Container* parent, const math::Transform& transform) :
        m_parentApp(parentApp), m_parent(parent), m_transform(transform) { }

    virtual ~Container() = default;

    /** @return A pointer to this Container's parent, or `nullptr` if it does not have a parent. */
    [[nodiscard]] Container* getParent() const { return m_parent; }

    /** @return A const reference to a std::vector containing this Container's children. */
    [[nodiscard]] const std::vector<std::unique_ptr<Container>>& getChildren() const { return m_children; }

    /**
     * Creates a child and adds it to this Container.
     * @tparam T The type of Container to create
     * @tparam Args The Container's constructor arguments
     * @param args The arguments to pass to the Container's constructor
     * @return A pointer to the newly created Container
     */
    template<typename T, typename... Args>
    T& createChild(Args&&... args) {
        auto childPtr = new T(m_parentApp, this, std::forward<Args>(args)...);
        m_children.emplace_back(childPtr);
        return *childPtr;
    }

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

    /** @return This Container's position */
    [[nodiscard]] math::Vec2 getPosition() const { return m_transform.position; }

    /** @param position The new position to set */
    void setPosition(math::Vec2 position);

    /** @return This Container's rotation, in radians */
    [[nodiscard]] float getRotation() const { return m_transform.rotation; }

    /** @param rotation The new rotation to set */
    void setRotation(float rotation);

    /** @param scale The new scale to set */
    void setScale(math::Vec2 scale);

    /** @param scale The new scale to set. Will be used for both x- and y-components. */
    void setScale(float scale);

    /** @param tint The new tint to set */
    void setTint(Color tint);

    /** @param pivot The center of rotation to set, as a normalized vector (components 0-1) */
    void setPivot(math::Vec2 pivot);

    /** @param visible Whether the Container should be visible */
    void setVisible(bool visible) { m_visible = visible; }

    /** @return The Transform applied to this Container (position, rotation, scale, etc.) */
    [[nodiscard]] const math::Transform& getTransform() const { return m_transform; }

    /** @return This Container's Z index */
    [[nodiscard]] int getZIndex() const { return m_zIndex; }

    /**
     * Sets the Z index of this Container.
     * IMPORTANT: This value will not be respected until sortChildren() is called on the parent.
     * @param zIndex The new Z index to set
     */
    void setZIndex(int zIndex) { m_zIndex = zIndex; }

    void animate(const AnimateOptions<math::Vec2>& opts);

    void animate(const AnimateOptions<float>& opts);

    /**
     * Renders the contents of this Container using the given @ref Renderer.
     * @param renderer The renderer to use
     */
    virtual void render(Renderer& renderer);
};

}
