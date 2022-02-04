#pragma once

#include <cmath>
#include <array>
#include "Vector2.h"

namespace engine { namespace math {

class Matrix3 {
public:
    std::array<float, 9> m; // column-major: [c0r0 c0r1 c0r2 c1r0 ...]

    Matrix3();
    explicit Matrix3(const float* data);

    float& operator()(int col, int row);
    float operator()(int col, int row) const;

    Matrix3 operator*(const Matrix3& other) const;
    Vector2f operator*(const Vector2f& v) const;
    Matrix3& operator*=(const Matrix3& other);

    static Matrix3 identity();
    static Matrix3 translation(float tx, float ty);
    static Matrix3 rotation(float angleRad);
    static Matrix3 scaling(float sx, float sy);
    static Matrix3 shear(float shx, float shy);

    Matrix3 transposed() const;
    float determinant() const;
    Matrix3 inverse() const;

    void setTranslation(float tx, float ty);
    void setRotation(float angleRad);
    void setScaling(float sx, float sy);

    Vector2f getTranslation() const;
    float getRotation() const;
    Vector2f getScale() const;

private:
    void setRow(int row, float a, float b, float c);
};

}} // namespace engine::math
