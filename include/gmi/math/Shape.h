#pragma once

#include "gmi/math/Vec2.h"
#include "gmi/math/collision.h"

#include <cassert>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace gmi::collision {

using namespace gmi::math;

class Shape {
public:
    enum Type : uint8_t {
        CIRCLE,
        RECT,
        POLYGON,
        COUNT
    };

    const Type type;

    [[nodiscard]] virtual std::string toString() const = 0;
    [[nodiscard]] virtual bool pointInside(Vec2f point) const = 0;

    [[nodiscard]] virtual Vec2f center() const = 0;

    virtual Shape& translate(Vec2f posToAdd) = 0;
    virtual Shape& scale(float scale) = 0;

    [[nodiscard]] virtual std::pair<Vec2f, Vec2f> getAABB() const = 0;

    virtual ~Shape() = default;

    Shape& operator=(const Shape&) = delete;
    Shape& operator=(Shape&&) noexcept = delete;

    /**
     * Check collision between this and another Shape.
     *
     * @return true if both shapes collide
     *
     * @param other The second shape
     * @param res A collision response pointer, optional / can be nullptr
     *
     * @note The collision response will only be valid if this function returned true.
     * @note The collision response normal will always be relative to this instance.
     *
     * @example
     * ```
     *   gmi::collision::Rect rect({10, 10}, {20, 20});
     *   gmi::collision::Circle circle({10, 15}, 1);
     *   gmi::collision::Response res;
     *   if (rect.getCollision(circle, &res)) {
     *       // this will separate the circle from the rectangle
     *       circle.translate(res.normal * res.depth);
     *   }
     * ```
     */
    [[nodiscard]] bool getCollision(const Shape& other, Response* res) const;

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape) {
        os << shape.toString();
        return os;
    }

protected:
    explicit Shape(Type type) : type(type) { }
};

class Circle : public Shape {
public:
    Vec2f pos;
    float rad;

    Circle(const Circle& circ);
    Circle(Circle&& circ) noexcept;
    Circle(Vec2f pos, float rad);

    Circle& operator=(Circle circ);

    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] bool pointInside(Vec2f point) const override;

    [[nodiscard]] Vec2f center() const override;

    Circle& translate(Vec2f posToAdd) override;
    Circle& scale(float scale) override;

    [[nodiscard]] std::pair<Vec2f, Vec2f> getAABB() const override;

private:
    static void swap(Circle& lhs, Circle& rhs) noexcept;
};

class Rect : public Shape {
public:
    Vec2f min;
    Vec2f max;

    Rect(const Rect&);
    Rect(Rect&&) noexcept;
    Rect(Vec2f min, Vec2f max);

    Rect& operator=(Rect rect);

    static Rect fromDims(float width, float height, Vec2f center = {0, 0});

    [[nodiscard]] float width() const {
        return max.x - min.x;
    }

    [[nodiscard]] float height() const {
        return max.y - min.y;
    }

    [[nodiscard]] std::vector<Vec2f> getPoints() const;

    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] bool pointInside(Vec2f point) const override;

    [[nodiscard]] Vec2f center() const override;

    Rect& translate(Vec2f posToAdd) override;
    Rect& scale(float scale) override;

    [[nodiscard]] std::pair<Vec2f, Vec2f> getAABB() const override;

private:
    static void swap(Rect& lhs, Rect& rhs) noexcept;
};

class Polygon : public Shape {
public:
    std::vector<Vec2f> points;

    explicit Polygon(std::vector<Vec2f> points);
    Polygon(const Polygon&);
    Polygon(Polygon&&) noexcept;

    Polygon& operator=(Polygon poly);

    static Polygon fromSides(size_t sides, Vec2f center, float radius);

    void calculateNormals();

    void calculateCenter();

    [[nodiscard]] const std::vector<Vec2f>& normals() const {
        return m_normals;
    };

    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] bool pointInside(Vec2f point) const override;

    [[nodiscard]] Vec2f center() const override;

    Polygon& translate(Vec2f posToAdd) override;
    Polygon& scale(float scale) override;

    Polygon& rotate(float rotation);

    [[nodiscard]] std::pair<Vec2f, Vec2f> getAABB() const override;
private:
    std::vector<Vec2f> m_normals;
    Vec2f m_center;

    static void swap(Polygon& lhs, Polygon& rhs) noexcept;
};

}
