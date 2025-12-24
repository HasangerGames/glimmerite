#pragma once

#include "gmi/math/Vec2.h"
#include "gmi/math/collision.h"

#include <cassert>
#include <cstdint>
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

    virtual std::pair<Vec2f, Vec2f> getAABB() = 0;

    virtual ~Shape() = default;

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

protected:
    explicit Shape(Type type) : type(type) { }
};

class Circle : public Shape {
public:
    Vec2f pos;
    float rad;

    Circle(Vec2f pos, float rad);

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] bool pointInside(Vec2f point) const override;

    [[nodiscard]] Vec2f center() const override;

    Circle& translate(Vec2f posToAdd) override;

    Circle& scale(float scale) override;

    std::pair<Vec2f, Vec2f> getAABB() override;
};

class Rect : public Shape {
public:
    Vec2f min;
    Vec2f max;

    Rect(Vec2f min, Vec2f max);

    static Rect fromDims(float width, float height, Vec2f center = {0, 0});

    [[nodiscard]] float width() const {
        return max.x - min.x;
    }

    [[nodiscard]] float height() const {
        return max.y - min.y;
    }

    [[nodiscard]] Vec2f center() const override;

    [[nodiscard]] std::vector<Vec2f> getPoints() const;

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] bool pointInside(Vec2f point) const override;

    Rect& translate(Vec2f posToAdd) override;

    Rect& scale(float scale) override;

    std::pair<Vec2f, Vec2f> getAABB() override;
};

class Polygon : public Shape {
public:
    std::vector<Vec2f> points;

private:
    std::vector<Vec2f> m_normals;
    Vec2f m_center;

public:
    explicit Polygon(const std::vector<Vec2f>& points);

    static Polygon fromSides(size_t sides, Vec2f center, float radius);

    void calculateNormals();

    void calculateCenter();

    [[nodiscard]] Vec2f center() const override;

    [[nodiscard]] const std::vector<Vec2f>& normals() const {
        return m_normals;
    };

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] bool pointInside(Vec2f point) const override;

    Polygon& translate(Vec2f posToAdd) override;

    Polygon& rotate(float rotation);

    Polygon& scale(float scale) override;

    std::pair<Vec2f, Vec2f> getAABB() override;
};

}
