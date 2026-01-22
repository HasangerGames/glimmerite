#include "gmi/client/Graphics.h"
#include "gmi/math/Shape.h"
#include <mapbox/earcut.hpp>
#include <utility>

// Makes earcut understand Vec2
namespace mapbox::util {

template<>
struct nth<0, gmi::math::Vec2f> {
    static auto get(const gmi::math::Vec2f& t) {
        return t.x;
    }
};

template<>
struct nth<1, gmi::math::Vec2f> {
    static auto get(const gmi::math::Vec2f& t) {
        return t.y;
    }
};

}

namespace gmi {

// Much of this code was adapted from Pixi.js:
// https://github.com/pixijs/pixijs/blob/dev/src/scene/graphics/shared/buildCommands/buildLine.ts

static float getOrientationOfPoints(const std::vector<math::Vec2f>& points) {
    if (points.size() < 3) {
        return 1;
    }

    float area = 0.0f;
    math::Vec2f prev = points.back();
    for (const math::Vec2f& point : points) {
        area += (point.x - prev.x) * (point.y + prev.y);
        prev = point;
    }

    return area < 0.0f ? -1 : 1;
}

/**
 * Buffers vertices to draw a square cap.
 * @param v End point
 * @param n Line normal pointing inside
 * @param innerWeight Weight of inner points
 * @param outerWeight Weight of outer points
 * @param clockwise Whether the cap is drawn clockwise
 * @param verts Vertex buffer
 * @return Number of vertices pushed
 */
void square(
    math::Vec2f v,
    math::Vec2f n,
    float innerWeight,
    float outerWeight,
    bool clockwise, // rotation for square (true at left end, false at right end)
    std::vector<math::Vec2f>& verts
) {
    math::Vec2f i = v - (n * innerWeight);
    math::Vec2f o = v + (n * outerWeight);

    // Rotate nx,ny for extension vector
    math::Vec2f e;
    if (clockwise) {
        e = {n.y, -n.x};
    } else {
        e = {-n.y, n.x};
    }

    // [i|0]x,y extended at cap
    math::Vec2f ei = i + e;
    math::Vec2f eo = o + e;

    // Square itself must be inserted clockwise
    verts.emplace_back(ei);
    verts.emplace_back(eo);
}

/**
 * Buffers vertices to draw an arc at the line joint or cap.
 * @param c Center
 * @param s Arc start
 * @param e Arc end
 * @param verts Buffer of vertices
 * @param clockwise Orientation of vertices
 * @return Number of vertices pushed
 */
void round(
    math::Vec2f c,
    math::Vec2f s,
    math::Vec2f e,
    std::vector<math::Vec2f>& verts,
    bool clockwise // if not cap, then clockwise is turn of joint, otherwise rotation from angle0 to angle1
) {
    math::Vec2f c2p0 = s - c;

    float angle0 = std::atan2(c2p0.x, c2p0.y);
    float angle1 = std::atan2(e.x - c.x, e.y - c.y);

    if (clockwise && angle0 < angle1) {
        angle0 += math::PI * 2;
    } else if (!clockwise && angle0 > angle1) {
        angle1 += math::PI * 2;
    }

    float startAngle = angle0;
    float angleDiff = angle1 - angle0;
    float absAngleDiff = std::abs(angleDiff);

    float radius = c2p0.length();
    int segCount = static_cast<int>(15 * absAngleDiff * std::sqrt(radius) / math::PI) + 1;
    float angleInc = angleDiff / static_cast<float>(segCount);

    startAngle += angleInc;

    if (clockwise) {
        verts.emplace_back(c);
        verts.emplace_back(s);

        float angle = startAngle;
        for (int i = 1; i < segCount; i++, angle += angleInc) {
            verts.emplace_back(c);
            verts.emplace_back(c.x + (std::sin(angle) * radius), c.y + (std::cos(angle) * radius));
        }

        verts.emplace_back(c);
        verts.emplace_back(e);
    } else {
        verts.emplace_back(s);
        verts.emplace_back(c);

        float angle = startAngle;
        for (int i = 1; i < segCount; i++, angle += angleInc) {
            verts.emplace_back(c.x + (std::sin(angle) * radius), c.y + (std::cos(angle) * radius));
            verts.emplace_back(c);
        }

        verts.emplace_back(e);
        verts.emplace_back(c);
    }
}

Graphics& Graphics::clear() {
    m_drawable.vertices.clear();
    m_drawable.indices.clear();
    return *this;
}

Graphics& Graphics::drawLine(std::vector<math::Vec2f> points, const StrokeStyle& style) {
    if (points.empty()) {
        return *this;
    }

    auto& [vertices, indices, _] = m_drawable;

    std::vector<math::Vec2f> verts;

    float alignment = style.alignment;
    if (alignment != 0.5f) {
        // rotate the points!
        float orientation = getOrientationOfPoints(points);
        alignment = ((alignment - 0.5f) * orientation) + 0.5f;
    }

    // get first and last point.. figure out the middle!
    math::Vec2f firstPoint = points.front();
    math::Vec2f lastPoint = points.back();

    // if the first point is the last point - gonna have issues :)
    if (style.closedShape) {
        bool closedPath = math::nearlyEqual(firstPoint.x, lastPoint.x) && math::nearlyEqual(firstPoint.y, lastPoint.y);
        if (closedPath) {
            points.resize(points.size() - 1);
            lastPoint = points.back();
        }

        math::Vec2f midPoint = (firstPoint + lastPoint) / 2.0f;

        points.insert(points.begin(), midPoint);
        points.emplace_back(midPoint);
    }

    // Max. inner and outer width
    float width = style.width / 2.0f;
    float widthSquared = width * width;
    uint16_t miterLimitSquared = style.miterLimit * style.miterLimit;

    // Line segments of interest where (x1,y1) forms the corner.
    math::Vec2f v0 = points[0],
                v1 = points[1],
                v2;

    // perp[?](x|y) = the line normal with magnitude lineWidth.
    math::Vec2f perp = {v1.y - v0.y, v0.x - v1.x};
    perp = perp / perp.length() * width;

    float ratio = alignment;
    float innerWeight = (1.0f - ratio) * 2.0f;
    float outerWeight = ratio * 2.0f;

    if (!style.closedShape) {
        if (style.cap == LineCap::Round) {
            round(
                v0 - (perp * (innerWeight - outerWeight) * 0.5f),
                v0 - (perp * innerWeight),
                v0 + (perp * outerWeight),
                verts,
                true
            );
        } else if (style.cap == LineCap::Square) {
            square(v0, perp, innerWeight, outerWeight, true, verts);
        }
    }

    // Push first point (below & above vertices)
    verts.emplace_back(v0 - (perp * innerWeight));
    verts.emplace_back(v0 + (perp * outerWeight));

    math::Vec2f perp1;

    size_t numPoints = points.size();
    for (size_t i = 1; i < numPoints - 1; i++) {
        v0 = points[i - 1];
        v1 = points[i];
        v2 = points[i + 1];

        perp = {v1.y - v0.y, v0.x - v1.x};
        perp = perp / perp.length() * width;

        perp1 = {v2.y - v1.y, v1.x - v2.x};
        perp1 = perp1 / perp1.length() * width;

        // d[x|y](0|1) = the component displacement between points p(0,1|1,2)
        math::Vec2f d0 = {v1.x - v0.x, v0.y - v1.y};
        math::Vec2f d1 = {v1.x - v2.x, v2.y - v1.y};

        // +ve if internal angle < 90 degree, -ve if internal angle > 90 degree.
        float dot = d0 * d1;
        // +ve if internal angle counterclockwise, -ve if internal angle clockwise.
        float cross = (d0.y * d1.x) - (d1.y * d0.x);
        bool clockwise = cross < 0;

        // Going nearly parallel?
        // atan(0.001) ~= 0.001 rad ~= 0.057 degree
        if (std::abs(cross) < 0.001 * std::abs(dot)) {
            verts.emplace_back(v1 - (perp * innerWeight));
            verts.emplace_back(v1 + (perp * outerWeight));

            // 180 degree corner?
            if (dot >= 0) {
                if (style.join == LineJoin::Round) {
                    round(
                        v1,
                        v1 - (perp * innerWeight),
                        v1 - (perp1 * innerWeight),
                        verts,
                        false
                    );
                }

                verts.emplace_back(v1 - (perp * outerWeight));
                verts.emplace_back(v1 + (perp * innerWeight));
            }

            continue;
        }

        // p[x|y] is the miter point. pDist is the distance between miter point and p1.
        float c1 = ((-perp.x + v0.x) * (-perp.y + v1.y)) - ((-perp.x + v1.x) * (-perp.y + v0.y));
        float c2 = ((-perp1.x + v2.x) * (-perp1.y + v1.y)) - ((-perp1.x + v1.x) * (-perp1.y + v2.y));
        math::Vec2f p = {
            ((d0.x * c2) - (d1.x * c1)) / cross,
            ((d1.y * c1) - (d0.y * c2)) / cross
        };
        float pDist = (p - v1).lengthSqr();

        // Inner miter point
        math::Vec2f im = v1 + ((p - v1) * innerWeight);
        // Outer miter point
        math::Vec2f om = v1 - ((p - v1) * outerWeight);

        // Is the inside miter point too far away, creating a spike?
        float smallerInsideSegmentSq = std::min(d0.lengthSqr(), d1.lengthSqr());
        float insideWeight = clockwise ? innerWeight : outerWeight;
        float smallerInsideDiagonalSq = smallerInsideSegmentSq + (insideWeight * insideWeight * widthSquared);
        bool insideMiterOk = pDist <= smallerInsideDiagonalSq;

        if (insideMiterOk) {
            if (style.join == LineJoin::Bevel || pDist / widthSquared > miterLimitSquared) {
                if (clockwise) { // rotating at inner angle
                    verts.emplace_back(im); // inner miter point
                    verts.emplace_back(v1 + (perp * outerWeight)); // first segment's outer vertex
                    verts.emplace_back(im); // inner miter point
                    verts.emplace_back(v1 + (perp1 * outerWeight)); // second segment's outer vertex
                } else { // rotating at outer angle
                    verts.emplace_back(v1 - (perp * innerWeight)); // first segment's inner vertex
                    verts.emplace_back(om); // outer miter point
                    verts.emplace_back(v1 - (perp1 * innerWeight)); // second segment's outer vertex
                    verts.emplace_back(om); // outer miter point
                }
            } else if (style.join == LineJoin::Round) {
                if (clockwise) { // arc is outside
                    verts.emplace_back(im);
                    verts.emplace_back(v1 + (perp * outerWeight));

                    round(
                        v1,
                        v1 + (perp * outerWeight),
                        v1 + (perp1 * outerWeight),
                        verts,
                        true
                    );

                    verts.emplace_back(im);
                    verts.emplace_back(v1 + (perp1 * outerWeight));
                } else { // arc is inside
                    verts.emplace_back(v1 - (perp * innerWeight));
                    verts.emplace_back(om);

                    round(
                        v1,
                        v1 - (perp * innerWeight),
                        v1 - (perp1 * innerWeight),
                        verts,
                        false
                    );

                    verts.emplace_back(v1 - (perp1 * innerWeight));
                    verts.emplace_back(om);
                }
            } else {
                verts.emplace_back(im);
                verts.emplace_back(om);
            }
        } else { // inside miter is NOT ok
            verts.emplace_back(v1 - (perp * innerWeight)); // first segment's inner vertex
            verts.emplace_back(v1 + (perp * outerWeight)); // first segment's outer vertex
            if (style.join == LineJoin::Round) {
                if (clockwise) { // arc is outside
                    round(
                        v1,
                        v1 + (perp * outerWeight),
                        v1 + (perp1 * outerWeight),
                        verts,
                        true
                    );
                } else { // arc is inside
                    round(
                        v1,
                        v1 - (perp * innerWeight),
                        v1 - (perp1 * innerWeight),
                        verts,
                        false
                    );
                }
            } else if (style.join == LineJoin::Miter && pDist / widthSquared <= miterLimitSquared) {
                if (clockwise) {
                    verts.emplace_back(om); // outer miter point
                    verts.emplace_back(om); // outer miter point
                } else {
                    verts.emplace_back(im); // inner miter point
                    verts.emplace_back(im); // inner miter point
                }
            }
            verts.emplace_back(v1 - (perp1 * innerWeight)); // second segment's inner vertex
            verts.emplace_back(v1 - (perp1 * outerWeight)); // second segment's outer vertex
        }
    }

    v0 = points[numPoints - 2];
    v1 = points[numPoints - 1];

    perp = {v1.y - v0.y, v0.x - v1.x};
    perp = perp / perp.lengthSqr() * width;

    verts.emplace_back(v1 - (perp * innerWeight));
    verts.emplace_back(v1 + (perp * outerWeight));

    if (!style.closedShape) {
        if (style.cap == LineCap::Round) {
            round(
                v1 - (perp * (innerWeight - outerWeight) * 0.5f),
                v1 - (perp * innerWeight),
                v1 + (perp * outerWeight),
                verts,
                false
            );
        } else if (style.cap == LineCap::Square) {
            square(v1, perp, innerWeight, outerWeight, false, verts);
        }
    }

    size_t totalVertices = vertices.size();
    size_t addedVertices = verts.size();
    vertices.reserve(totalVertices + addedVertices);
    indices.reserve(indices.size() + addedVertices); // some are skipped so this reserves more than needed, but still helps performance

    static constexpr float CURVE_EPS = 0.0001;
    static constexpr float EPS2 = CURVE_EPS * CURVE_EPS;
    for (size_t i = 0, len = verts.size() - 2; i < len; i++) {
        v0 = verts[i];
        v1 = verts[i + 1];
        v2 = verts[i + 2];

        // Skip zero area triangles
        if (std::abs((v0.x * (v1.y - v2.y)) + (v1.x * (v2.y - v0.y)) + (v2.x * (v0.y - v1.y))) < EPS2) {
            continue;
        }

        indices.emplace_back(totalVertices + i);
        indices.emplace_back(totalVertices + i + 1);
        indices.emplace_back(totalVertices + i + 2);
    }

    for (auto [x, y] : verts) {
        vertices.emplace_back(x, y, 0, 0, style.color);
    }

    return *this;
}

Graphics& Graphics::fillRect(float x, float y, float w, float h, Color color) {
    auto& [vertices, indices, _] = m_drawable;

    size_t numVertices = vertices.size();
    indices.reserve(indices.size() + 6);
    indices.emplace_back(numVertices + 0);
    indices.emplace_back(numVertices + 1);
    indices.emplace_back(numVertices + 2);
    indices.emplace_back(numVertices + 0);
    indices.emplace_back(numVertices + 2);
    indices.emplace_back(numVertices + 3);

    vertices.reserve(numVertices + 4);
    // clang-format off
    vertices.emplace_back(x,     y,     0, 1, color); // Top left
    vertices.emplace_back(x + w, y,     1, 1, color); // Top right
    vertices.emplace_back(x + w, y + h, 1, 0, color); // Bottom right
    vertices.emplace_back(x,     y + h, 0, 0, color); // Bottom left
    // clang-format on

    return *this;
}

Graphics& Graphics::fillCircle(float x, float y, float r, Color color) {
    return fillEllipse(x, y, r, r, color);
}

Graphics& Graphics::fillEllipse(float x, float y, float rx, float ry, Color color) {
    auto& [vertices, indices, _] = m_drawable;

    // Choose a number of segments such that the maximum absolute deviation from the circle is approximately 0.029
    size_t numSegments = std::ceil(2.3 * std::sqrt(rx + ry));
    float angleInc = math::TAU / static_cast<float>(numSegments);

    size_t numVertices = vertices.size();
    vertices.reserve(numVertices + numSegments + 1);
    indices.reserve(indices.size() + numSegments * 3);

    vertices.emplace_back(x, y, 0, 0, color); // center vertex

    for (size_t i = 0; i <= numSegments; i++) {
        float angle = static_cast<float>(i) * angleInc;
        vertices.emplace_back(x + rx * std::cos(angle), y + ry * std::sin(angle), 0, 0, color);

        indices.emplace_back(numVertices); // index referring to center vertex
        indices.emplace_back(numVertices + i);
        indices.emplace_back(numVertices + i + 1);
    }

    return *this;
}

Graphics& Graphics::fillPoly(const std::vector<math::Vec2f>& points, Color color) {
    auto& [vertices, indices, texture] = m_drawable;

    std::vector<uint16_t> polyIndices = mapbox::earcut<uint16_t>(std::vector<std::vector<math::Vec2f>>{points});
    size_t numVertices = vertices.size();
    indices.reserve(indices.size() + polyIndices.size());
    for (uint16_t index : polyIndices) {
        indices.emplace_back(numVertices + index);
    }

    vertices.reserve(numVertices + points.size());
    for (auto [x, y] : points) {
        vertices.emplace_back(x, y, 0, 0, color);
    }

    return *this;
}

Graphics& Graphics::fillShape(const collision::Shape& shape, Color color) {
    switch (shape.type) {
    case collision::Shape::CIRCLE: {
        const auto& circle = static_cast<const collision::Circle&>(shape);
        return fillCircle(circle.pos.x, circle.pos.y, circle.rad, color);
    }
    case collision::Shape::RECT: {
        const auto& rect = static_cast<const collision::Rect&>(shape);
        return fillRect(rect.min.x, rect.min.y, rect.width(), rect.height(), color);
    }
    case collision::Shape::POLYGON: {
        const auto& poly = static_cast<const collision::Polygon&>(shape);
        return fillPoly(poly.points, color);
    }
    default:
        // if it ever reaches this theres probably random uninitialized memory lol
        std::unreachable();
    }
}

void Graphics::render(Renderer& renderer) {
    Container::render(renderer);

    renderer.queueDrawable(m_drawable);
}

}
