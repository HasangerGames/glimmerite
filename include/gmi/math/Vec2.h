#pragma once

#include <cmath>
#include <cstdint>
#include <format>
#include <string>
#include <type_traits>

#define VEC2_EPSILON 0.001f

namespace gmi::math {

template<typename VecT>
class Vec2 {
public:
    VecT x;
    VecT y;

    Vec2() : x(0), y(0) { }

    Vec2(VecT xAndY) : x(xAndY), y(xAndY) { }

    Vec2(VecT xPos, VecT yPos) : x(xPos), y(yPos) { }

    [[nodiscard]] static Vec2 left() {
        return {-1, 0};
    }

    [[nodiscard]] static Vec2 right() {
        return {1, 0};
    }

    [[nodiscard]] static Vec2 down() {
        return {0, -1};
    }

    [[nodiscard]] static Vec2 up() {
        return {0, 1};
    }

    [[nodiscard]] Vec2 clone() const {
        return {x, y};
    }

    void set(VecT xPos, VecT yPos) {
        x = xPos;
        y = yPos;
    }

    [[nodiscard]] bool isValid() const
        requires(std::is_floating_point_v<VecT>)
    {
        return std::isfinite(x) && std::isfinite(y);
    }

    Vec2& rotate(float rad) {
        const VecT cosr = std::cos(rad);
        const VecT sinr = std::sin(rad);

        x = x * cosr - y * sinr;
        y = x * sinr + y * cosr;

        return *this;
    }

    Vec2& invert() {
        x = -x;
        y = -y;
        return *this;
    }

    Vec2& perp() {
        VecT oldX = x;
        x = -y;
        y = oldX;
        return *this;
    }

    Vec2& normalize() {
        const VecT len = length();

        if (len > VEC2_EPSILON) {
            x /= len;
            y /= len;
        }

        return *this;
    }

    Vec2& normalize(VecT length) {
        if (length > VEC2_EPSILON) {
            x /= length;
            y /= length;
        }

        return *this;
    }

    Vec2& normalizeSafe(const Vec2& v = {1, 0}) {
        const VecT len = length();

        if (len > VEC2_EPSILON) {
            x /= len;
            y /= len;
        } else {
            x = v.x;
            y = v.y;
        }

        return *this;
    }

    [[nodiscard]] VecT lengthSqr() const {
        return x * x + y * y;
    }

    [[nodiscard]] VecT length() const {
        return std::sqrt(lengthSqr());
    }

    [[nodiscard]] VecT distanceTo(const Vec2& a) const {
        return (*this - a).length();
    }

    [[nodiscard]] VecT dot(const Vec2& a) const {
        return x * a.x + y * a.y;
    }

    [[nodiscard]] static Vec2 min(const Vec2& a, const Vec2& b) {
        return {std::min(a.x, b.x), std::min(a.y, b.y)};
    }

    [[nodiscard]] static Vec2 max(const Vec2& a, const Vec2& b) {
        return {std::max(a.x, b.x), std::max(a.y, b.y)};
    }

    [[nodiscard]] bool equals(const Vec2& a) const {
        return x == a.x && y == a.y;
    }

    [[nodiscard]] bool equals(const Vec2& a, VecT epsilon) const
        requires(std::is_floating_point_v<VecT>)
    {
        return std::abs(x - a.x) <= epsilon && std::abs(y - a.y) <= epsilon;
    }

    bool operator==(const Vec2& a) const {
        return equals(a);
    }

    bool operator!=(const Vec2& a) const {
        return !equals(a);
    }

    [[nodiscard]] VecT operator[](uint8_t index) const {
        return (&x)[index];
    }

    VecT& operator[](uint8_t index) {
        return (&x)[index];
    }

    [[nodiscard]] Vec2 operator-() const {
        return {-x, -y};
    }

    VecT operator*(const Vec2& a) const {
        return x * a.x + y * a.y;
    }

    [[nodiscard]] Vec2 operator*(VecT a) const {
        return {x * a, y * a};
    }

    [[nodiscard]] Vec2 operator/(VecT a) const {
        return {x / a, y / a};
    }

    [[nodiscard]] Vec2 operator+(const Vec2& a) const {
        return {x + a.x, y + a.y};
    }

    [[nodiscard]] Vec2 operator-(const Vec2& a) const {
        return {x - a.x, y - a.y};
    }

    Vec2& operator+=(const Vec2& a) {
        x += a.x;
        y += a.y;

        return *this;
    }

    Vec2& operator-=(const Vec2& a) {
        x -= a.x;
        y -= a.y;

        return *this;
    }

    Vec2& operator/=(const Vec2& a) {
        x /= a.x;
        y /= a.y;

        return *this;
    }

    Vec2& operator/=(VecT a) {
        x /= a;
        y /= a;

        return *this;
    }

    Vec2& operator*=(VecT a) {
        x *= a;
        y *= a;

        return *this;
    }

    [[nodiscard]] std::string toString() const
        requires(std::is_floating_point_v<VecT>)
    {
        return std::format("X: {0:.4f}, Y: {1:.4f}", x, y);
    }

    [[nodiscard]] std::string toString() const
        requires(std::is_integral_v<VecT>)
    {
        return std::format("X:    {}, Y:    {}", x, y);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
        os << vec.toString();
        return os;
    }
};

using Vec2f = Vec2<float>;
using Vec2u = Vec2<uint32_t>;

}
