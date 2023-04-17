#include "Matrix4.h"
#include <cmath>
#include <algorithm>

namespace engine { namespace math {

Matrix4::Matrix4() {
    std::fill(m.begin(), m.end(), 0.0f);
}

Matrix4::Matrix4(const float* data) {
    std::copy(data, data + 16, m.begin());
}

float& Matrix4::operator()(int col, int row) {
    return m[col * 4 + row];
}

float Matrix4::operator()(int col, int row) const {
    return m[col * 4 + row];
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += (*this)(k, r) * other(c, k);
            }
            result(c, r) = sum;
        }
    }
    return result;
}

Vector3f Matrix4::operator*(const Vector3f& v) const {
    float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];
    if (std::abs(w) < 1e-10f) return Vector3f();
    float invW = 1.0f / w;
    return Vector3f(
        (m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12]) * invW,
        (m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13]) * invW,
        (m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]) * invW
    );
}

Matrix4& Matrix4::operator*=(const Matrix4& other) {
    *this = *this * other;
    return *this;
}

Matrix4 Matrix4::identity() {
    Matrix4 mat;
    mat.m[0] = 1.0f; mat.m[5] = 1.0f; mat.m[10] = 1.0f; mat.m[15] = 1.0f;
    return mat;
}

Matrix4 Matrix4::translation(float tx, float ty, float tz) {
    Matrix4 mat = identity();
    mat.m[12] = tx;
    mat.m[13] = ty;
    mat.m[14] = tz;
    return mat;
}

Matrix4 Matrix4::rotationX(float angleRad) {
    Matrix4 mat = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    mat.m[5] = c;  mat.m[9] = -s;
    mat.m[6] = s;  mat.m[10] = c;
    return mat;
}

Matrix4 Matrix4::rotationY(float angleRad) {
    Matrix4 mat = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    mat.m[0] = c;  mat.m[8] = s;
    mat.m[2] = -s; mat.m[10] = c;
    return mat;
}

Matrix4 Matrix4::rotationZ(float angleRad) {
    Matrix4 mat = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    mat.m[0] = c;  mat.m[4] = -s;
    mat.m[1] = s;  mat.m[5] = c;
    return mat;
}

Matrix4 Matrix4::scaling(float sx, float sy, float sz) {
    Matrix4 mat = identity();
    mat.m[0] = sx;
    mat.m[5] = sy;
    mat.m[10] = sz;
    return mat;
}

Matrix4 Matrix4::perspective(float fovRad, float aspect, float near, float far) {
    Matrix4 mat;
    float f = 1.0f / std::tan(fovRad * 0.5f);
    float rangeInv = 1.0f / (near - far);
    mat.m[0] = f / aspect;
    mat.m[5] = f;
    mat.m[10] = (near + far) * rangeInv;
    mat.m[11] = -1.0f;
    mat.m[14] = 2.0f * near * far * rangeInv;
    return mat;
}

Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 mat = identity();
    float rml = right - left;
    float tmb = top - bottom;
    float fmn = far - near;
    mat.m[0] = 2.0f / rml;
    mat.m[5] = 2.0f / tmb;
    mat.m[10] = -2.0f / fmn;
    mat.m[12] = -(right + left) / rml;
    mat.m[13] = -(top + bottom) / tmb;
    mat.m[14] = -(far + near) / fmn;
    return mat;
}

Matrix4 Matrix4::lookAt(const Vector3f& eye, const Vector3f& target, const Vector3f& up) {
    Vector3f f = (target - eye).normalized();
    Vector3f s = f.cross(up).normalized();
    Vector3f u = s.cross(f);

    Matrix4 mat = identity();
    mat.m[0] = s.x;  mat.m[4] = s.y;  mat.m[8]  = s.z;
    mat.m[1] = u.x;  mat.m[5] = u.y;  mat.m[9]  = u.z;
    mat.m[2] = -f.x; mat.m[6] = -f.y; mat.m[10] = -f.z;
    mat.m[12] = -s.dot(eye);
    mat.m[13] = -u.dot(eye);
    mat.m[14] = f.dot(eye);
    return mat;
}

Matrix4 Matrix4::transposed() const {
    Matrix4 result;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            result(c, r) = (*this)(r, c);
        }
    }
    return result;
}

float Matrix4::determinant() const {
    auto sub = [&](int c0, int c1, int c2, int r0, int r1, int r2) -> float {
        return (*this)(c0, r0) * ((*this)(c1, r1) * (*this)(c2, r2) - (*this)(c1, r2) * (*this)(c2, r1))
             - (*this)(c0, r1) * ((*this)(c1, r0) * (*this)(c2, r2) - (*this)(c1, r2) * (*this)(c2, r0))
             + (*this)(c0, r2) * ((*this)(c1, r0) * (*this)(c2, r1) - (*this)(c1, r1) * (*this)(c2, r0));
    };
    float a0 = m[0] * sub(1, 2, 3, 1, 2, 3);
    float a1 = m[4] * sub(0, 2, 3, 1, 2, 3);
    float a2 = m[8] * sub(0, 1, 3, 1, 2, 3);
    float a3 = m[12] * sub(0, 1, 2, 1, 2, 3);
    return a0 - a1 + a2 - a3;
}

Matrix4 Matrix4::inverse() const {
    float det = determinant();
    if (std::abs(det) < 1e-10f) return identity();
    float invDet = 1.0f / det;
    Matrix4 inv;
    auto cof = [&](int c0, int c1, int c2, int r0, int r1, int r2) -> float {
        return ((*this)(c0, r0) * ((*this)(c1, r1) * (*this)(c2, r2) - (*this)(c1, r2) * (*this)(c2, r1))
              - (*this)(c0, r1) * ((*this)(c1, r0) * (*this)(c2, r2) - (*this)(c1, r2) * (*this)(c2, r0))
              + (*this)(c0, r2) * ((*this)(c1, r0) * (*this)(c2, r1) - (*this)(c1, r1) * (*this)(c2, r0)));
    };
    inv.m[0] =  cof(1, 2, 3, 1, 2, 3) * invDet;
    inv.m[1] = -cof(0, 2, 3, 1, 2, 3) * invDet;
    inv.m[2] =  cof(0, 1, 3, 1, 2, 3) * invDet;
    inv.m[3] = -cof(0, 1, 2, 1, 2, 3) * invDet;
    inv.m[4] = -cof(1, 2, 3, 0, 2, 3) * invDet;
    inv.m[5] =  cof(0, 2, 3, 0, 2, 3) * invDet;
    inv.m[6] = -cof(0, 1, 3, 0, 2, 3) * invDet;
    inv.m[7] =  cof(0, 1, 2, 0, 2, 3) * invDet;
    inv.m[8] =  cof(1, 2, 3, 0, 1, 3) * invDet;
    inv.m[9] = -cof(0, 2, 3, 0, 1, 3) * invDet;
    inv.m[10] = cof(0, 1, 3, 0, 1, 3) * invDet;
    inv.m[11] = -cof(0, 1, 2, 0, 1, 3) * invDet;
    inv.m[12] = -cof(1, 2, 3, 0, 1, 2) * invDet;
    inv.m[13] = cof(0, 2, 3, 0, 1, 2) * invDet;
    inv.m[14] = -cof(0, 1, 3, 0, 1, 2) * invDet;
    inv.m[15] = cof(0, 1, 2, 0, 1, 2) * invDet;
    return inv;
}

}} // namespace engine::math

