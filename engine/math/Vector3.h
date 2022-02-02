#pragma once

#include <cmath>
#include <cassert>

namespace engine { namespace math {

template <typename T>
class Vector3 {
public:
    T x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    explicit Vector3(T s) : x(s), y(s), z(s) {}

    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(T s) const { return Vector3(x * s, y * s, z * s); }
    Vector3 operator/(T s) const { assert(s != 0); return Vector3(x / s, y / s, z / s); }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector3& operator*=(T s) { x *= s; y *= s; z *= s; return *this; }
    Vector3& operator/=(T s) { assert(s != 0); x /= s; y /= s; z /= s; return *this; }

    bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vector3& v) const { return x != v.x || y != v.y || z != v.z; }

    T dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    T lengthSq() const { return x * x + y * y + z * z; }
    T length() const { return static_cast<T>(std::sqrt(lengthSq())); }

    Vector3 normalized() const {
        T len = length();
        if (len < static_cast<T>(1e-10)) return Vector3();
        return *this / len;
    }

    Vector3& normalize() {
        T len = length();
        if (len < static_cast<T>(1e-10)) { x = y = z = 0; return *this; }
        return *this /= len;
    }

    Vector3 lerp(const Vector3& target, T t) const {
        return *this + (target - *this) * t;
    }

    static Vector3 zero() { return Vector3(); }
    static Vector3 one() { return Vector3(1, 1, 1); }
    static Vector3 forward() { return Vector3(0, 0, -1); }
    static Vector3 back() { return Vector3(0, 0, 1); }
    static Vector3 up() { return Vector3(0, 1, 0); }
    static Vector3 down() { return Vector3(0, -1, 0); }
    static Vector3 left() { return Vector3(-1, 0, 0); }
    static Vector3 right() { return Vector3(1, 0, 0); }
};

template <typename T>
Vector3<T> operator*(T s, const Vector3<T>& v) {
    return Vector3<T>(s * v.x, s * v.y, s * v.z);
}

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3d = Vector3<double>;

}} // namespace engine::math
