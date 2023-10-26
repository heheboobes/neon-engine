#pragma once

#include <array>
#include "Vector3.h"

namespace engine { namespace math {

class Matrix4 {
public:
    std::array<float, 16> m;

    Matrix4();
    explicit Matrix4(const float* data);

    float& operator()(int col, int row);
    float operator()(int col, int row) const;

    Matrix4 operator*(const Matrix4& other) const;
    Vector3f operator*(const Vector3f& v) const;
    Matrix4& operator*=(const Matrix4& other);

    static Matrix4 identity();
    static Matrix4 translation(float tx, float ty, float tz);
    static Matrix4 rotationX(float angleRad);
    static Matrix4 rotationY(float angleRad);
    static Matrix4 rotationZ(float angleRad);
    static Matrix4 scaling(float sx, float sy, float sz);

    static Matrix4 perspective(float fovRad, float aspect, float near, float far);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 lookAt(const Vector3f& eye, const Vector3f& target, const Vector3f& up);

    Matrix4 transposed() const;
    float determinant() const;
    Matrix4 inverse() const;
};

}} // namespace engine::math
// v2: Clang Wconversion fixes
