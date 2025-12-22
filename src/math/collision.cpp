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
    const Vec2f& normal,
    const Vec2f& center,

    float* out_min,
    float* out_max
) {
    float min = std::numeric_limits<float>::max();
    float max = -min;

    for (const Vec2f& point : points) {
        float proj = normal * (center - point);

        min = std::min(proj, min);
        max = std::max(proj, max);
    }

    *out_min = min;
    *out_max = max;
}

static void projectCircle(
    const Vec2f& center,
    const float radius,
    const Vec2f& normal,

    float* out_min,
    float* out_max
) {
    Vec2f scaled = normal * radius;

    Vec2f p1 = center + scaled;
    Vec2f p2 = center - scaled;

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
    const Vec2f& posA,
    const float radA,

    const Vec2f& posB,
    const float radB,

    CollRes* res
) {
    Vec2f sub = posB - posA;

    float distSqrt = sub.lengthSqr();
    float rad = radA + radB;

    if (distSqrt > (rad * rad)) {
        return false;
    }

    if (res != nullptr) {
        float dist = std::sqrt(distSqrt);
        res->normal = dist > 0.000001 ? sub / dist : Vec2f(1, 0);
        res->depth = rad - dist;
    }
    return true;
}

bool CircleRect(
    const Vec2f& circlePos,
    const float circleRad,

    const Vec2f& rectMin,
    const Vec2f& rectMax,

    CollRes* res
) {
    if (
        rectMin.x <= circlePos.x && circlePos.x <= rectMax.x
        && rectMin.y <= circlePos.y && circlePos.y <= rectMax.y
    ) {
        // circle inside rect
        if (res != nullptr) {
            Vec2f halfDimension = (rectMax - rectMin) * 0.5;
            Vec2f rectToCircle = (rectMin + halfDimension) - circlePos;
            float xDepth = std::abs(rectToCircle.x) - halfDimension.x - circleRad;
            float yDepth = std::abs(rectToCircle.y) - halfDimension.y - circleRad;

            // make the normal relative to the deepest axis
            if (xDepth > yDepth) {
                res->normal = Vec2f(
                    rectToCircle.x > 0 ? 1 : -1,
                    0
                );
                res->depth = -xDepth;
            } else {
                res->normal = Vec2f(
                    0,
                    rectToCircle.y > 0 ? 1 : -1
                );
                res->depth = -yDepth;
            }
        }

        return true;
    }

    Vec2f dir{
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
    const Vec2f& rectAMin,
    const Vec2f& rectAMax,

    const Vec2f& rectBMin,
    const Vec2f& rectBMax,

    CollRes* res
) {
    // if the caller doesn't want the intersection data
    // do a simpler check
    if (res == nullptr) {
        return rectBMin.x < rectBMax.x && rectBMin.y < rectBMax.y && rectAMin.x < rectAMax.x && rectAMin.y < rectAMax.y;
    }

    Vec2f halfDimA = (rectAMax - rectAMin) * 0.5;
    Vec2f halfDimB = (rectBMax - rectBMin) * 0.5;

    Vec2f bToA = ((rectBMin + halfDimB) - (rectAMin + halfDimA));

    float xDepth = halfDimA.x + halfDimB.x - std::abs(bToA.x);

    if (xDepth <= 0)
        return false;

    float yDepth = halfDimA.y + halfDimB.y - std::abs(bToA.y);

    if (yDepth <= 0)
        return false;

    if (xDepth < yDepth) {
        res->normal = Vec2f(
            bToA.x < 0 ? -1 : 1,
            0
        );
        res->depth = xDepth;
    } else {
        res->normal = Vec2f(
            0,
            bToA.y < 0 ? -1 : 1
        );
        res->depth = yDepth;
    }

    return true;
}

bool RectPolygon(
    const Vec2f& rectMin,
    const Vec2f& rectMax,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    const Vec2f& polyCenter,

    CollRes* res
) {
    assert(polyPoints.size() == polyNormals.size());

    const std::array<Vec2f, 4> rectPoints = {
        rectMin,
        {rectMin.x, rectMax.y},
        rectMax,
        {rectMax.x, rectMin.y}
    };

    static const std::array<Vec2f, 4> rectNormals{
        Vec2f{0, 1},
        Vec2f{-1, 0},
        Vec2f{0, -1},
        Vec2f{1, 0}
    };

    Vec2f rectCenter = rectMin + ((rectMax - rectMin) / 2);

    bool wantsRes = res != nullptr;
    Vec2f resNormal;
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
        Vec2f direction = rectCenter - polyCenter;

        if (direction * resNormal > 0) {
            resNormal.invert();
        }

        res->normal = resNormal;
        res->depth = resDepth;
    }

    return true;
}

bool CirclePolygon(
    const Vec2f& circlePos,
    float circleRad,

    const std::vector<Vec2f>& polyPoints,
    const std::vector<Vec2f>& polyNormals,
    const Vec2f& polyCenter,

    CollRes* res
) {
    assert(polyPoints.size() == polyNormals.size());

    Vec2f circToPoly = polyCenter - circlePos;

    Vec2f closestPoint;
    float minDist = std::numeric_limits<float>::max();

    bool wantsRes = res != nullptr;
    Vec2f resNormal;
    float resDepth = std::numeric_limits<float>::max();

    for (size_t i = 0; i < polyPoints.size(); i++) {
        const Vec2f& normal = polyNormals[i];
        const Vec2f& point = polyPoints[i];

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

    Vec2f normal = (closestPoint - circlePos).normalize();

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

        Vec2f direction = polyCenter - circlePos;

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
    const std::vector<Vec2f>& pointsA,
    const std::vector<Vec2f>& normalsA,
    const Vec2f& centerA,

    const std::vector<Vec2f>& pointsB,
    const std::vector<Vec2f>& normalsB,
    const Vec2f& centerB,

    CollRes* res
) {
    assert(pointsA.size() == normalsA.size());
    assert(pointsB.size() == normalsB.size());

    bool wantsRes = res != nullptr;
    Vec2f resNormal;
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
        Vec2f direction = centerB - centerA;

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

bool PointCircle(const Vec2f& point, const Vec2f& circlePos, float circleRad) {
    return point.distanceTo(circlePos) <= circleRad;
}

bool PointRect(const Vec2f& point, const Vec2f& rectMin, const Vec2f& rectMax) {
    return point.x > rectMin.x && point.y > rectMin.y && point.x < rectMax.x && point.y < rectMax.y;
}

bool PointPolygon(const Vec2f& point, const std::vector<Vec2f>& points) {
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
