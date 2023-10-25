#pragma once

#include <cmath>
#include <cassert>

namespace engine { namespace math {

template <typename T>
class Vector2 {
public:
    T x, y;

    Vector2() : x(0), y(0) {}
    Vector2(T x, T y) : x(x), y(y) {}
    explicit Vector2(T s) : x(s), y(s) {}

    Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
    Vector2 operator*(T s) const { return Vector2(x * s, y * s); }
    Vector2 operator/(T s) const { assert(s != 0); return Vector2(x / s, y / s); }
    Vector2 operator-() const { return Vector2(-x, -y); }

    Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
    Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    Vector2& operator*=(T s) { x *= s; y *= s; return *this; }
    Vector2& operator/=(T s) { assert(s != 0); x /= s; y /= s; return *this; }

    bool operator==(const Vector2& v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vector2& v) const { return x != v.x || y != v.y; }

    T dot(const Vector2& v) const { return x * v.x + y * v.y; }
    T cross(const Vector2& v) const { return x * v.y - y * v.x; }
    T lengthSq() const { return x * x + y * y; }
    T length() const { return static_cast<T>(std::sqrt(lengthSq())); }

    Vector2 normalized() const {
        T len = length();
        if (len < static_cast<T>(1e-10)) return Vector2();
        return *this / len;
    }

    Vector2& normalize() {
        T len = length();
        if (len < static_cast<T>(1e-10)) { x = y = 0; return *this; }
        return *this /= len;
    }

    Vector2 lerp(const Vector2& target, T t) const {
        return *this + (target - *this) * t;
    }

    Vector2 rotated(T angleRad) const {
        T c = static_cast<T>(std::cos(angleRad));
        T s = static_cast<T>(std::sin(angleRad));
        return Vector2(x * c - y * s, x * s + y * c);
    }

    Vector2& rotate(T angleRad) {
        T c = static_cast<T>(std::cos(angleRad));
        T s = static_cast<T>(std::sin(angleRad));
        T nx = x * c - y * s;
        y = x * s + y * c;
        x = nx;
        return *this;
    }

    static Vector2 zero() { return Vector2(); }
    static Vector2 one() { return Vector2(1, 1); }
    static Vector2 left() { return Vector2(-1, 0); }
    static Vector2 right() { return Vector2(1, 0); }
    static Vector2 up() { return Vector2(0, -1); }
    static Vector2 down() { return Vector2(0, 1); }
};

template <typename T>
Vector2<T> operator*(T s, const Vector2<T>& v) {
    return Vector2<T>(s * v.x, s * v.y);
}

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2d = Vector2<double>;

}} // namespace engine::math
// v2: Added rotate and lerp methods
// DOCS: Math library docs
// v2: GCC Wconversion fixes
