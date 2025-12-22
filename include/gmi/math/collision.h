#pragma once

#include "gmi/math/Vec2.h"

#include <vector>

namespace gmi::collision {
using namespace gmi::math;

/**
 *  Collision response between 2 shapes.
 *
 *  To separate them you can move them by the normal multiplied by the depth
 */
struct CollRes {
    /**
     * The direction to move the shapes so they separate
     */
    Vec2f normal;
    /**
     * How much the shapes are colliding
     */
    float depth;
};

bool CircleCircle(
    const Vec2f& posA,
    float radA,

    const Vec2f& posB,
    float radB,

    CollRes* res
);

bool CircleRect(
    const Vec2f& circlePos,
    float circleRad,

    const Vec2f& rectMin,
    const Vec2f& rectMax,

    CollRes* res
);

bool CirclePolygon(
    const Vec2f& circlePos,
    float circleRad,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    const Vec2f& polyCenter,

    CollRes* res
);

bool RectRect(
    const Vec2f& rectAMin,
    const Vec2f& rectAMax,

    const Vec2f& rectBMin,
    const Vec2f& rectBMax,

    CollRes* res
);

bool RectPolygon(
    const Vec2f& rectMin,
    const Vec2f& rectMax,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    const Vec2f& polyCenter,

    CollRes* res
);

bool PolygonPolygon(
    const std::vector<Vec2f>& pointsA,
    const std::vector<Vec2f>& normalsA,
    const Vec2f& centerA,

    const std::vector<Vec2f>& pointsB,
    const std::vector<Vec2f>& normalsB,
    const Vec2f& centerB,

    CollRes* res
);

bool PointCircle(const Vec2f& point, const Vec2f& circlePos, float circleRad);

bool PointRect(const Vec2f& point, const Vec2f& rectMin, const Vec2f& rectMax);

bool PointPolygon(const Vec2f& point, const std::vector<Vec2f>& points);

};
