#pragma once

#include "Vec2.h"

#include <vector>

namespace gmi::collision {

using namespace gmi::math;

/**
 * Collision response between 2 shapes.
 * To separate them, you can move them by the normal multiplied by the depth.
 */
struct Response {
    /** The direction to move the shapes so they separate. */
    Vec2f normal;
    /** How much the shapes are colliding. */
    float depth;
};

bool circleCircle(
    const Vec2f& posA,
    float radA,

    const Vec2f& posB,
    float radB,

    Response* res
);

bool circleRect(
    const Vec2f& circlePos,
    float circleRad,

    const Vec2f& rectMin,
    const Vec2f& rectMax,

    Response* res
);

bool circlePolygon(
    const Vec2f& circlePos,
    float circleRad,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    const Vec2f& polyCenter,

    Response* res
);

bool rectRect(
    const Vec2f& rectAMin,
    const Vec2f& rectAMax,

    const Vec2f& rectBMin,
    const Vec2f& rectBMax,

    Response* res
);

bool rectPolygon(
    const Vec2f& rectMin,
    const Vec2f& rectMax,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    const Vec2f& polyCenter,

    Response* res
);

bool polygonPolygon(
    const std::vector<Vec2f>& pointsA,
    const std::vector<Vec2f>& normalsA,
    const Vec2f& centerA,

    const std::vector<Vec2f>& pointsB,
    const std::vector<Vec2f>& normalsB,
    const Vec2f& centerB,

    Response* res
);

bool pointCircle(const Vec2f& point, const Vec2f& circlePos, float circleRad);

bool pointRect(const Vec2f& point, const Vec2f& rectMin, const Vec2f& rectMax);

bool pointPolygon(const Vec2f& point, const std::vector<Vec2f>& points);

};
