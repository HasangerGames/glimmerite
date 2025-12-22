#include "gmi/math/collision.h"

using namespace gmi::math;
using namespace gmi::collision;

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

template<typename T>
static void projectVertices(
    const T& points,
    const Vec2F& normal,
    const Vec2F& center,

    float* out_min,
    float* out_max
) {
    float min = std::numeric_limits<float>::max();
    float max = -min;

    for (const Vec2F& point : points) {
        float proj = normal * (center - point);

        min = std::min(proj, min);
        max = std::max(proj, max);
    }

    *out_min = min;
    *out_max = max;
}

static void projectCircle(
    const Vec2F& center,
    const float radius,
    const Vec2F& normal,

    float* out_min,
    float* out_max
) {
    Vec2F scaled = normal * radius;

    Vec2F p1 = center + scaled;
    Vec2F p2 = center - scaled;

    float min = p1 * normal;
    float max = p2 * normal;

    if (min > max) {
        // swap the min and max values.
        float t = min;
        min = max;
        max = t;
    }

    *out_min = min;
    *out_max = max;
}

namespace gmi::collision {

bool CircleCircle(
    const Vec2F& posA,
    const float radA,

    const Vec2F& posB,
    const float radB,

    CollRes* res
) {
    Vec2F sub = posB - posA;

    float distSqrt = sub.lengthSqr();
    float rad = radA + radB;

    if (distSqrt > (rad * rad)) {
        return false;
    }

    if (res != nullptr) {
        float dist = std::sqrt(distSqrt);
        res->normal = dist > 0.000001 ? sub / dist : Vec2F(1, 0);
        res->depth = rad - dist;
    }
    return true;
}

bool CircleRect(
    const Vec2F& circlePos,
    const float circleRad,

    const Vec2F& rectMin,
    const Vec2F& rectMax,

    CollRes* res
) {
    if (
        rectMin.x <= circlePos.x && circlePos.x <= rectMax.x
        && rectMin.y <= circlePos.y && circlePos.y <= rectMax.y
    ) {
        // circle inside rect
        if (res != nullptr) {
            Vec2F halfDimension = (rectMax - rectMin) * 0.5;
            Vec2F rectToCircle = (rectMin + halfDimension) - circlePos;
            float xDepth = std::abs(rectToCircle.x) - halfDimension.x - circleRad;
            float yDepth = std::abs(rectToCircle.y) - halfDimension.y - circleRad;

            // make the normal relative to the deepest axis
            if (xDepth > yDepth) {
                res->normal = Vec2F(
                    rectToCircle.x > 0 ? 1 : -1,
                    0
                );
                res->depth = -xDepth;
            } else {
                res->normal = Vec2F(
                    0,
                    rectToCircle.y > 0 ? 1 : -1
                );
                res->depth = -yDepth;
            }
        }

        return true;
    }

    Vec2F dir{
        std::clamp(circlePos.x, rectMin.x, rectMax.x) - circlePos.x,
        std::clamp(circlePos.y, rectMin.y, rectMax.y) - circlePos.y
    };

    float dstSqr = dir.lengthSqr();

    if (dstSqr < circleRad * circleRad) {
        if (res != nullptr) {
            float dst = std::sqrt(dstSqr);

            res->normal = dir.normalize(dst);
            res->depth = circleRad - dst;
        }

        return true;
    }

    return false;
}

bool RectRect(
    const Vec2F& rectAMin,
    const Vec2F& rectAMax,

    const Vec2F& rectBMin,
    const Vec2F& rectBMax,

    CollRes* res
) {
    // if the caller doesn't want the intersection data
    // do a simpler check
    if (res == nullptr) {
        return rectBMin.x < rectBMax.x && rectBMin.y < rectBMax.y && rectAMin.x < rectAMax.x && rectAMin.y < rectAMax.y;
    }

    Vec2F halfDimA = (rectAMax - rectAMin) * 0.5;
    Vec2F halfDimB = (rectBMax - rectBMin) * 0.5;

    Vec2F bToA = ((rectBMin + halfDimB) - (rectAMin + halfDimA));

    float xDepth = halfDimA.x + halfDimB.x - std::abs(bToA.x);

    if (xDepth <= 0)
        return false;

    float yDepth = halfDimA.y + halfDimB.y - std::abs(bToA.y);

    if (yDepth <= 0)
        return false;

    if (xDepth < yDepth) {
        res->normal = Vec2F(
            bToA.x < 0 ? -1 : 1,
            0
        );
        res->depth = xDepth;
    } else {
        res->normal = Vec2F(
            0,
            bToA.y < 0 ? -1 : 1
        );
        res->depth = yDepth;
    }

    return true;
}

bool RectPolygon(
    const Vec2F& rectMin,
    const Vec2F& rectMax,

    const std::vector<Vec2F>& polyPoints,
    const std::vector<Vec2F>& polyNormals,
    const Vec2F& polyCenter,

    CollRes* res
) {
    assert(polyPoints.size() == polyNormals.size());

    const std::array<Vec2F, 4> rectPoints = {
        rectMin,
        {rectMin.x, rectMax.y},
        rectMax,
        {rectMax.x, rectMin.y}
    };

    static const std::array<Vec2F, 4> rectNormals{
        Vec2F{0, 1},
        Vec2F{-1, 0},
        Vec2F{0, -1},
        Vec2F{1, 0}
    };

    Vec2F rectCenter = rectMin + ((rectMax - rectMin) / 2);

    bool wantsRes = res != nullptr;
    Vec2F resNormal;
    float resDepth = std::numeric_limits<float>::max();

    for (const auto& vertNormal : polyNormals) {
        float minA, maxA, minB, maxB;
        projectVertices(polyPoints, vertNormal, {}, &minA, &maxA);
        projectVertices(rectPoints, vertNormal, {}, &minB, &maxB);

        if (minA >= maxB || minB >= maxA) {
            return false;
        }

        if (wantsRes) {
            float axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < resDepth) {
                resDepth = axisDepth;
                resNormal = vertNormal;
            }
        }
    }

    for (const auto& vertNormal : rectNormals) {
        float minA, maxA, minB, maxB;
        projectVertices(polyPoints, vertNormal, {}, &minA, &maxA);
        projectVertices(rectPoints, vertNormal, {}, &minB, &maxB);

        if (minA >= maxB || minB >= maxA) {
            return false;
        }

        if (wantsRes) {
            float axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < resDepth) {
                resDepth = axisDepth;
                resNormal = vertNormal;
            }
        }
    }

    if (wantsRes) {
        Vec2F direction = rectCenter - polyCenter;

        if (direction * resNormal > 0) {
            resNormal.invert();
        }

        res->normal = resNormal;
        res->depth = resDepth;
    }

    return true;
}

bool CirclePolygon(
    const Vec2F& circlePos,
    float circleRad,

    const std::vector<Vec2F>& polyPoints,
    const std::vector<Vec2F>& polyNormals,
    const Vec2F& polyCenter,

    CollRes* res
) {
    assert(polyPoints.size() == polyNormals.size());

    Vec2F circToPoly = polyCenter - circlePos;

    Vec2F closestPoint;
    float minDist = std::numeric_limits<float>::max();

    bool wantsRes = res != nullptr;
    Vec2F resNormal;
    float resDepth = std::numeric_limits<float>::max();

    for (size_t i = 0; i < polyPoints.size(); i++) {
        const Vec2F& normal = polyNormals[i];
        const Vec2F& point = polyPoints[i];

        float minA, maxA, minB, maxB;
        projectVertices(polyPoints, normal, polyCenter, &minA, &maxA);
        projectCircle(circToPoly, circleRad, normal, &minB, &maxB);

        if (minA >= maxB || minB >= maxA) {
            return false;
        }

        if (wantsRes) {
            float depth = std::min(maxB - minA, maxA - minB);

            if (depth < resDepth) {
                resDepth = depth;
                resNormal = normal;
            }

            float dist = point.distanceTo(circlePos);
            if (dist < minDist) {
                minDist = dist;
                closestPoint = point;
            }
        }
    }

    Vec2F normal = (closestPoint - circlePos).normalize();

    float minA, maxA, minB, maxB;
    projectVertices(polyPoints, normal, polyCenter, &minA, &maxA);
    projectCircle(circToPoly, circleRad, normal, &minB, &maxB);

    if (minA >= maxB || minB >= maxA) {
        return false;
    }

    if (wantsRes) {
        float depth = std::min(maxB - minA, maxA - minB);

        if (depth < resDepth) {
            resDepth = depth;
            resNormal = normal;
        }

        Vec2F direction = polyCenter - circlePos;

        if ((direction * resNormal) < 0) {
            resNormal.invert();
        }

        if (res != nullptr) {
            res->normal = resNormal;
            res->depth = resDepth;
        }
    }

    return true;
}

bool PolygonPolygon(
    const std::vector<Vec2F>& pointsA,
    const std::vector<Vec2F>& normalsA,
    const Vec2F& centerA,

    const std::vector<Vec2F>& pointsB,
    const std::vector<Vec2F>& normalsB,
    const Vec2F& centerB,

    CollRes* res
) {
    assert(pointsA.size() == normalsA.size());
    assert(pointsB.size() == normalsB.size());

    bool wantsRes = res != nullptr;
    Vec2F resNormal;
    float resDepth = std::numeric_limits<float>::max();

    for (const auto& vertNormal : normalsA) {
        float minA, maxA, minB, maxB;
        projectVertices(pointsA, vertNormal, {}, &minA, &maxA);
        projectVertices(pointsB, vertNormal, {}, &minB, &maxB);

        if (minA >= maxB || minB >= maxA) {
            return false;
        }

        if (wantsRes) {
            float axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < resDepth) {
                resDepth = axisDepth;
                resNormal = vertNormal;
            }
        }
    }

    for (const auto& vertNormal : normalsB) {
        float minA, maxA, minB, maxB;
        projectVertices(pointsA, vertNormal, {}, &minA, &maxA);
        projectVertices(pointsB, vertNormal, {}, &minB, &maxB);

        if (minA >= maxB || minB >= maxA) {
            return false;
        }

        if (wantsRes) {
            float axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < resDepth) {
                resDepth = axisDepth;
                resNormal = vertNormal;
            }
        }
    }

    if (wantsRes) {
        Vec2F direction = centerB - centerA;

        if (direction * resNormal < 0) {
            resNormal.invert();
        }

        if (res != nullptr) {
            res->normal = resNormal;
            res->depth = resDepth;
        }
    }
    return true;
}

bool PointCircle(const Vec2F& point, const Vec2F& circlePos, float circleRad) {
    return point.distanceTo(circlePos) <= circleRad;
}

bool PointRect(const Vec2F& point, const Vec2F& rectMin, const Vec2F& rectMax) {
    return point.x > rectMin.x && point.y > rectMin.y && point.x < rectMax.x && point.y < rectMax.y;
}

bool PointPolygon(const Vec2F& point, const std::vector<Vec2F>& points) {
    // https://wrfranklin.org/Research/Short_Notes/pnpoly.html
    size_t count = points.size();
    bool inside = false;

    for (size_t i = 0, j = count - 1; i < count; j = i++) {
        if (
            ((points[i].y >= point.y) != (points[j].y >= point.y))
            && (point.x <= (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
        ) {
            inside = !inside;
        }
    }

    return inside;
};

}
