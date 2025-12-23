#include "gmi/math/Shape.h"
#include <cfloat>

using namespace gmi::math;
using namespace gmi::collision;

using CollisionFn = bool (*)(const Shape&, const Shape&, Response*);

struct CollisionFnData {
    CollisionFn fn;
    bool reverse;
};

class CollisionFns {
private:
    std::array<std::array<CollisionFnData, Shape::COUNT>, Shape::COUNT> m_fns;

    void registerFn(Shape::Type typeA, Shape::Type typeB, const CollisionFn& fn) {
        m_fns[typeA][typeB] = {
            .fn = fn,
            .reverse = false
        };

        if (typeA != typeB) {
            m_fns[typeB][typeA] = {
                .fn = fn,
                .reverse = true
            };
        }
    };

public:
    CollisionFns() {
        registerFn(Shape::CIRCLE, Shape::CIRCLE, [](const Shape& shapeA, const Shape& shapeB, auto* res) {
            assert(shapeA.type == Shape::CIRCLE);
            assert(shapeB.type == Shape::CIRCLE);

            const auto& a = static_cast<const Circle&>(shapeA);
            const auto& b = static_cast<const Circle&>(shapeB);
            return circleCircle(a.pos, a.rad, b.pos, b.rad, res);
        });
        registerFn(Shape::CIRCLE, Shape::RECT, [](const Shape& shapeA, const Shape& shapeB, auto* res) {
            assert(shapeA.type == Shape::CIRCLE);
            assert(shapeB.type == Shape::RECT);

            const auto& a = static_cast<const Circle&>(shapeA);
            const auto& b = static_cast<const Rect&>(shapeB);
            return circleRect(a.pos, a.rad, b.min, b.max, res);
        });
        registerFn(Shape::CIRCLE, Shape::POLYGON, [](const Shape& shapeA, const Shape& shapeB, auto* res) {
            assert(shapeA.type == Shape::CIRCLE);
            assert(shapeB.type == Shape::POLYGON);

            const auto& a = static_cast<const Circle&>(shapeA);
            const auto& b = static_cast<const Polygon&>(shapeB);
            return circlePolygon(a.pos, a.rad, b.points, b.normals(), b.center(), res);
        });
        registerFn(Shape::RECT, Shape::RECT, [](const Shape& shapeA, const Shape& shapeB, auto* res) {
            assert(shapeA.type == Shape::RECT);
            assert(shapeB.type == Shape::RECT);

            const auto& a = static_cast<const Rect&>(shapeA);
            const auto& b = static_cast<const Rect&>(shapeB);
            return rectRect(a.min, a.max, b.min, b.max, res);
        });
        registerFn(Shape::RECT, Shape::POLYGON, [](const Shape& shapeA, const Shape& shapeB, auto* res) {
            assert(shapeA.type == Shape::RECT);
            assert(shapeB.type == Shape::POLYGON);

            const auto& a = static_cast<const Rect&>(shapeA);
            const auto& b = static_cast<const Polygon&>(shapeB);
            return rectPolygon(a.min, a.max, b.points, b.normals(), b.center(), res);
        });
        registerFn(Shape::POLYGON, Shape::POLYGON, [](const Shape& shapeA, const Shape& shapeB, auto* res) {
            assert(shapeA.type == Shape::POLYGON);
            assert(shapeB.type == Shape::POLYGON);

            const auto& a = static_cast<const Polygon&>(shapeA);
            const auto& b = static_cast<const Polygon&>(shapeB);
            return polygonPolygon(a.points, a.normals(), a.center(), b.points, b.normals(), b.center(), res);
        });
    };

    bool check(const Shape& shapeA, const Shape& shapeB, Response* res) const {
        assert(shapeA.type < Shape::COUNT);
        assert(shapeB.type < Shape::COUNT);

        const auto& collisionFn = m_fns[shapeA.type][shapeB.type];

        if (collisionFn.reverse) {
            bool collided = collisionFn.fn(shapeB, shapeA, res);
            res->normal.invert();
            return collided;
        }

        return collisionFn.fn(shapeA, shapeB, res);
    };
};

namespace gmi::collision {

bool Shape::getCollision(const Shape& other, Response* res) const {
    static const CollisionFns fns;
    return fns.check(*this, other, res);
}

Circle::Circle(Vec2f pos, float rad) :
    Shape(CIRCLE),
    pos(pos),
    rad(rad) {
    assert(rad >= 0);
}

std::string Circle::toString() const {
    return std::format("Circle (X: {0:.4f}, Y: {1:.4f}, Rad: {2:.4f})", pos.x, pos.y, rad);
}

bool Circle::pointInside(Vec2f point) const {
    return pointCircle(point, pos, rad);
}

Vec2f Circle::center() const {
    return pos;
}

Circle& Circle::translate(Vec2f posToAdd) {
    pos += posToAdd;
    return *this;
}

Circle& Circle::scale(const float scale) {
    rad *= scale;
    return *this;
}

std::pair<Vec2f, Vec2f> Circle::getAABB() {
    return {
        {pos.x - rad, pos.y - rad},
        {pos.x + rad, pos.y + rad}
    };
};

Rect::Rect(Vec2f min, Vec2f max) :
    Shape(RECT),
    min(min),
    max(max) {
    // min being bigger than max can cause issues with collision functions
    assert(min.x < max.x);
    assert(min.y < max.y);
}

Rect Rect::fromDims(float width, float height, Vec2f center) {
    Vec2f size{width / 2, height / 2};

    return Rect{center - size, center + size};
}

Rect& Rect::scale(const float scale) {
    Vec2f center = this->center();

    min = (min - center) * scale + center;
    max = (max - center) * scale + center;

    return *this;
}

std::pair<Vec2f, Vec2f> Rect::getAABB() {
    return {min, max};
};

Rect& Rect::translate(Vec2f posToAdd) {
    min += posToAdd;
    max += posToAdd;

    return *this;
}

bool Rect::pointInside(Vec2f point) const {
    return pointRect(point, min, max);
}

std::string Rect::toString() const {
    return std::format("Rect(Min ({}) Max ({}))", min.toString(), max.toString());
}

std::vector<Vec2f> Rect::getPoints() const {
    return {
        min,
        {min.x, max.y},
        max,
        {max.x, min.y}
    };
}

Vec2f Rect::center() const {
    return min + ((max - min) / 2);
}

Polygon::Polygon(const std::vector<Vec2f>& points) :
    Shape(POLYGON),
    points(points),
    m_normals(points.size()) {
    assert(points.size() >= 3);

    calculateNormals();
    calculateCenter();
}

Polygon& Polygon::scale(const float scale) {
    for (auto& pt : points) {
        Vec2f toCenter = m_center - pt;
        float length = toCenter.length();
        Vec2f dir = toCenter.normalize(length);

        pt = m_center - (dir * (length * scale));
    }

    return *this;
}

Polygon& Polygon::translate(Vec2f posToAdd) {
    for (auto& point : points) {
        point += posToAdd;
    }
    m_center += posToAdd;

    return *this;
}

std::pair<Vec2f, Vec2f> Polygon::getAABB() {
    Vec2f min{FLT_MAX, FLT_MAX};
    Vec2f max{-FLT_MAX, -FLT_MAX};
    for (Vec2f& pt : points) {
        min = Vec2f::min(pt, min);
        max = Vec2f::max(pt, max);
    }
    return {min, max};
};

Polygon& Polygon::rotate(float rotation) {
    for (auto& point : points) {
        float dist = point.distanceTo(m_center);

        Vec2f dir = (point - m_center).normalize().rotate(rotation).normalize();

        point = m_center - (dir * dist);
    }

    calculateCenter();
    calculateNormals();

    return *this;
};

bool Polygon::pointInside(Vec2f point) const {
    return pointPolygon(point, points);
}

std::string Polygon::toString() const {
    std::string out = "Polygon [";

    for (size_t i = 0, size = points.size(); i < size; i++) {
        out += std::format("({})", points[i].toString());
        if (i != size - 1) {
            out += ", ";
        }
    }
    out += "]";

    return out;
}

Vec2f Polygon::center() const {
    return m_center;
}

void Polygon::calculateCenter() {
    m_center = {0, 0};
    for (const auto& point : points) {
        m_center += point;
    }
    m_center /= points.size();
}

void Polygon::calculateNormals() {
    for (size_t i = 0; i < points.size(); i++) {
        Vec2f pointA = points[i];
        Vec2f pointB = points[(i + 1) % points.size()];
        Vec2f edge = pointB - pointA;

        m_normals[i] = edge.perp().normalize();
    }
}

}
