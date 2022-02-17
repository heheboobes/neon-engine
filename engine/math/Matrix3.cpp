#include "Matrix3.h"
#include <stdexcept>

namespace engine { namespace math {

Matrix3::Matrix3() {
    std::fill(m.begin(), m.end(), 0.0f);
}

Matrix3::Matrix3(const float* data) {
    std::copy(data, data + 9, m.begin());
}

float& Matrix3::operator()(int col, int row) {
    return m[col * 3 + row];
}

float Matrix3::operator()(int col, int row) const {
    return m[col * 3 + row];
}

Matrix3 Matrix3::operator*(const Matrix3& other) const {
    Matrix3 result;
    for (int c = 0; c < 3; ++c) {
        for (int r = 0; r < 3; ++r) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) {
                sum += (*this)(k, r) * other(c, k);
            }
            result(c, r) = sum;
        }
    }
    return result;
}

Vector2f Matrix3::operator*(const Vector2f& v) const {
    float w = m[2] * v.x + m[5] * v.y + m[8];
    if (std::abs(w) < 1e-10f) return Vector2f();
    return Vector2f(
        (m[0] * v.x + m[3] * v.y + m[6]) / w,
        (m[1] * v.x + m[4] * v.y + m[7]) / w
    );
}

Matrix3& Matrix3::operator*=(const Matrix3& other) {
    *this = *this * other;
    return *this;
}

Matrix3 Matrix3::identity() {
    Matrix3 mat;
    mat.m[0] = 1.0f; mat.m[4] = 1.0f; mat.m[8] = 1.0f;
    return mat;
}

Matrix3 Matrix3::translation(float tx, float ty) {
    Matrix3 mat = identity();
    mat.m[6] = tx;
    mat.m[7] = ty;
    return mat;
}

Matrix3 Matrix3::rotation(float angleRad) {
    Matrix3 mat = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    mat.m[0] = c;   mat.m[3] = -s;
    mat.m[1] = s;   mat.m[4] = c;
    return mat;
}

Matrix3 Matrix3::scaling(float sx, float sy) {
    Matrix3 mat = identity();
    mat.m[0] = sx;
    mat.m[4] = sy;
    return mat;
}

Matrix3 Matrix3::shear(float shx, float shy) {
    Matrix3 mat = identity();
    mat.m[3] = shx;
    mat.m[1] = shy;
    return mat;
}

Matrix3 Matrix3::transposed() const {
    Matrix3 result;
    for (int c = 0; c < 3; ++c) {
        for (int r = 0; r < 3; ++r) {
            result(c, r) = (*this)(r, c);
        }
    }
    return result;
}

float Matrix3::determinant() const {
    return m[0] * (m[4] * m[8] - m[7] * m[5])
         - m[3] * (m[1] * m[8] - m[7] * m[2])
         + m[6] * (m[1] * m[5] - m[4] * m[2]);
}

Matrix3 Matrix3::inverse() const {
    float det = determinant();
    if (std::abs(det) < 1e-10f) return identity();

    float invDet = 1.0f / det;
    Matrix3 inv;
    inv.m[0] = (m[4] * m[8] - m[7] * m[5]) * invDet;
    inv.m[1] = (m[7] * m[2] - m[1] * m[8]) * invDet;
    inv.m[2] = (m[1] * m[5] - m[4] * m[2]) * invDet;
    inv.m[3] = (m[6] * m[5] - m[3] * m[8]) * invDet;
    inv.m[4] = (m[0] * m[8] - m[6] * m[2]) * invDet;
    inv.m[5] = (m[3] * m[2] - m[0] * m[5]) * invDet;
    inv.m[6] = (m[3] * m[7] - m[6] * m[4]) * invDet;
    inv.m[7] = (m[6] * m[1] - m[0] * m[7]) * invDet;
    inv.m[8] = (m[0] * m[4] - m[3] * m[1]) * invDet;
    return inv;
}

void Matrix3::setTranslation(float tx, float ty) {
    m[6] = tx; m[7] = ty;
}

void Matrix3::setRotation(float angleRad) {
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    m[0] = c; m[3] = -s;
    m[1] = s; m[4] = c;
}

void Matrix3::setScaling(float sx, float sy) {
    m[0] = sx; m[4] = sy;
}

Vector2f Matrix3::getTranslation() const {
    return Vector2f(m[6], m[7]);
}

float Matrix3::getRotation() const {
    return std::atan2(m[1], m[0]);
}

Vector2f Matrix3::getScale() const {
    float sx = std::sqrt(m[0] * m[0] + m[1] * m[1]);
    float sy = std::sqrt(m[3] * m[3] + m[4] * m[4]);
    return Vector2f(sx, sy);
}

void Matrix3::setRow(int row, float a, float b, float c) {
    m[row] = a; m[3 + row] = b; m[6 + row] = c;
}

}} // namespace engine::math
// v2: Optimized multiplication
