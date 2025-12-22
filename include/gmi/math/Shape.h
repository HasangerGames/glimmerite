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
    [[nodiscard]] virtual bool pointInside(const Vec2F& point) const = 0;

    [[nodiscard]] virtual Vec2F center() const = 0;

    virtual Shape& translate(const Vec2F& posToAdd) = 0;
    virtual Shape& scale(float scale) = 0;

    virtual std::pair<Vec2F, Vec2F> getAABB() = 0;

    virtual ~Shape() = default;

    /**
     * Check collision between this and another Shape.
     *
     * @return true if both shapes collide.
     *
     * @param Other The second shape.
     * @param res A collision response pointer, optional / can be nullptr.
     *
     * @note The collision response will only be valid if this function returned true.
     * @note The collision response normal will always be relative to this instance.
     *
     * @example
     * ```
     *   gmi::collision::Rect rect({10, 10}, {20, 20});
     *   gmi::collision::Circle circle({10, 15}, 1);
     *   gmi::collision::CollRes res;
     *   if (rect.getCollision(circle, &res)) {
     *       // this will separate the circle from the rectangle
     *       circle.translate(res.normal * res.depth);
     *   }
     * ```
     */
    [[nodiscard]] bool getCollision(const Shape& other, CollRes* res) const;

protected:
    Shape(Type type) :
        type(type) {
    }
};

class Circle : public Shape {
public:
    Vec2F pos;
    float rad;

    Circle(Vec2F pos, float rad);

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] bool pointInside(const Vec2F& point) const override;

    [[nodiscard]] Vec2F center() const override;

    Circle& translate(const Vec2F& posToAdd) override;

    Circle& scale(float scale) override;

    std::pair<Vec2F, Vec2F> getAABB() override;
};

class Rect : public Shape {
public:
    Vec2F min;
    Vec2F max;

    Rect(Vec2F min, Vec2F max);

    static Rect fromDims(float width, float height, Vec2F center = {0, 0});

    [[nodiscard]] float width() const {
        return max.x - min.x;
    }

    [[nodiscard]] float height() const {
        return max.y - min.y;
    }

    [[nodiscard]] Vec2F center() const override;

    [[nodiscard]] std::vector<Vec2F> getPoints() const;

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] bool pointInside(const Vec2F& point) const override;

    Rect& translate(const Vec2F& posToAdd) override;

    Rect& scale(float scale) override;

    std::pair<Vec2F, Vec2F> getAABB() override;
};

class Polygon : public Shape {
public:
    std::vector<Vec2F> points;

private:
    std::vector<Vec2F> m_normals;
    Vec2F m_center;

public:
    Polygon(const std::vector<Vec2F>& points);

    void calculateNormals();

    void calculateCenter();

    [[nodiscard]] Vec2F center() const override;

    [[nodiscard]] const std::vector<Vec2F>& normals() const {
        return m_normals;
    };

    [[nodiscard]] std::string toString() const override;

    [[nodiscard]] bool pointInside(const Vec2F& point) const override;

    Polygon& translate(const Vec2F& posToAdd) override;

    Polygon& rotate(float rotation);

    Polygon& scale(float scale) override;

    std::pair<Vec2F, Vec2F> getAABB() override;
};

}
