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
    Vec2f posA,
    float radA,

    Vec2f posB,
    float radB,

    Response* res
);

bool circleRect(
    Vec2f circlePos,
    float circleRad,

    Vec2f rectMin,
    Vec2f rectMax,

    Response* res
);

bool circlePolygon(
    Vec2f circlePos,
    float circleRad,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    Vec2f polyCenter,

    Response* res
);

bool rectRect(
    Vec2f rectAMin,
    Vec2f rectAMax,

    Vec2f rectBMin,
    Vec2f rectBMax,

    Response* res
);

bool rectPolygon(
    Vec2f rectMin,
    Vec2f rectMax,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    Vec2f polyCenter,

    Response* res
);

bool polygonPolygon(
    const std::vector<Vec2f>& pointsA,
    const std::vector<Vec2f>& normalsA,
    Vec2f centerA,

    const std::vector<Vec2f>& pointsB,
    const std::vector<Vec2f>& normalsB,
    Vec2f centerB,

    Response* res
);

bool pointCircle(Vec2f point, Vec2f circlePos, float circleRad);

bool pointRect(Vec2f point, Vec2f rectMin, Vec2f rectMax);

bool pointPolygon(Vec2f point, const std::vector<Vec2f>& points);

};
