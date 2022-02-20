#include "../main.cpp"
#include <engine/math/Matrix4.h>
#include <engine/math/Vector3.h>
#include <cmath>
#include <cassert>

namespace tests::math
{

TEST(Matrix, Identity)
{
    engine::math::Matrix4 m = engine::math::Matrix4::identity();

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            assert(m(i, j) == (i == j ? 1.0f : 0.0f));

    return true;
}

TEST(Matrix, Multiplication)
{
    auto t = engine::math::Matrix4::translation(2.0f, 3.0f, 4.0f);
    auto s = engine::math::Matrix4::scale(2.0f, 2.0f, 2.0f);
    engine::math::Matrix4 combined = t * s;

    assert(std::abs(combined(0, 0) - 2.0f) < 0.0001f);
    assert(std::abs(combined(1, 1) - 2.0f) < 0.0001f);
    assert(std::abs(combined(2, 2) - 2.0f) < 0.0001f);
    assert(std::abs(combined(0, 3) - 2.0f) < 0.0001f);
    assert(std::abs(combined(1, 3) - 3.0f) < 0.0001f);
    assert(std::abs(combined(2, 3) - 4.0f) < 0.0001f);

    return true;
}

TEST(Matrix, Translation)
{
    engine::math::Matrix4 m = engine::math::Matrix4::translation(5.0f, 10.0f, 15.0f);

    assert(std::abs(m(0, 3) - 5.0f) < 0.0001f);
    assert(std::abs(m(1, 3) - 10.0f) < 0.0001f);
    assert(std::abs(m(2, 3) - 15.0f) < 0.0001f);

    return true;
}

TEST(Matrix, RotationX)
{
    engine::math::Matrix4 m = engine::math::Matrix4::rotationX(3.14159f / 2.0f);
    engine::math::Vector3 v(0.0f, 1.0f, 0.0f);
    engine::math::Vector3 r = m.transform(v);

    assert(std::abs(r.x) < 0.0001f);
    assert(std::abs(r.y) < 0.0001f);
    assert(std::abs(r.z - 1.0f) < 0.0001f);

    return true;
}

TEST(Matrix, Scale)
{
    engine::math::Matrix4 m = engine::math::Matrix4::scale(2.0f, 3.0f, 4.0f);
    engine::math::Vector3 v(1.0f, 1.0f, 1.0f);
    engine::math::Vector3 r = m.transform(v);

    assert(std::abs(r.x - 2.0f) < 0.0001f);
    assert(std::abs(r.y - 3.0f) < 0.0001f);
    assert(std::abs(r.z - 4.0f) < 0.0001f);

    return true;
}

TEST(Matrix, Inverse)
{
    auto t = engine::math::Matrix4::translation(10.0f, 20.0f, 30.0f);
    engine::math::Matrix4 inv = t.inverse();

    engine::math::Matrix4 result = t * inv;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            assert(std::abs(result(i, j) - (i == j ? 1.0f : 0.0f)) < 0.0001f);

    return true;
}

TEST(Matrix, Determinant)
{
    engine::math::Matrix4 m = engine::math::Matrix4::identity();
    float det = m.determinant();
    assert(std::abs(det - 1.0f) < 0.0001f);

    auto s = engine::math::Matrix4::scale(2.0f, 3.0f, 4.0f);
    det = s.determinant();
    assert(std::abs(det - 24.0f) < 0.0001f);

    return true;
}

TEST(Matrix, TransformPoint)
{
    auto m = engine::math::Matrix4::translation(1.0f, 2.0f, 3.0f);
    engine::math::Vector3 point(0.0f, 0.0f, 0.0f);
    engine::math::Vector3 result = m.transform(point);

    assert(std::abs(result.x - 1.0f) < 0.0001f);
    assert(std::abs(result.y - 2.0f) < 0.0001f);
    assert(std::abs(result.z - 3.0f) < 0.0001f);

    return true;
}

}
