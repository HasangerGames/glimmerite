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
    Vec2F normal;
    /**
     * How much the shapes are colliding
     */
    float depth;
};

bool CircleCircle(
    const Vec2F& posA,
    float radA,

    const Vec2F& posB,
    float radB,

    CollRes* res
);

bool CircleRect(
    const Vec2F& circlePos,
    float circleRad,

    const Vec2F& rectMin,
    const Vec2F& rectMax,

    CollRes* res
);

bool CirclePolygon(
    const Vec2F& circlePos,
    float circleRad,

    const std::vector<Vec2F>& polyPoints,
    const std::vector<Vec2F>& polyNormals,
    const Vec2F& polyCenter,

    CollRes* res
);

bool RectRect(
    const Vec2F& rectAMin,
    const Vec2F& rectAMax,

    const Vec2F& rectBMin,
    const Vec2F& rectBMax,

    CollRes* res
);

bool RectPolygon(
    const Vec2F& rectMin,
    const Vec2F& rectMax,

    const std::vector<Vec2F>& polyPoints,
    const std::vector<Vec2F>& polyNormals,
    const Vec2F& polyCenter,

    CollRes* res
);

bool PolygonPolygon(
    const std::vector<Vec2F>& pointsA,
    const std::vector<Vec2F>& normalsA,
    const Vec2F& centerA,

    const std::vector<Vec2F>& pointsB,
    const std::vector<Vec2F>& normalsB,
    const Vec2F& centerB,

    CollRes* res
);

bool PointCircle(const Vec2F& point, const Vec2F& circlePos, float circleRad);

bool PointRect(const Vec2F& point, const Vec2F& rectMin, const Vec2F& rectMax);

bool PointPolygon(const Vec2F& point, const std::vector<Vec2F>& points);

};
