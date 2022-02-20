#include "../main.cpp"
#include <engine/math/Vector3.h>
#include <cmath>
#include <cassert>

namespace tests::math
{

TEST(Vector3, DefaultConstructor)
{
    engine::math::Vector3 v;
    assert(v.x == 0.0f);
    assert(v.y == 0.0f);
    assert(v.z == 0.0f);
    return true;
}

TEST(Vector3, ParameterizedConstructor)
{
    engine::math::Vector3 v(1.0f, 2.0f, 3.0f);
    assert(v.x == 1.0f);
    assert(v.y == 2.0f);
    assert(v.z == 3.0f);
    return true;
}

TEST(Vector3, CrossProduct)
{
    engine::math::Vector3 a(1.0f, 0.0f, 0.0f);
    engine::math::Vector3 b(0.0f, 1.0f, 0.0f);
    engine::math::Vector3 c = a.cross(b);
    assert(std::abs(c.x - 0.0f) < 0.0001f);
    assert(std::abs(c.y - 0.0f) < 0.0001f);
    assert(std::abs(c.z - 1.0f) < 0.0001f);
    return true;
}

TEST(Vector3, DotProduct)
{
    engine::math::Vector3 a(2.0f, 3.0f, 4.0f);
    engine::math::Vector3 b(1.0f, 0.0f, -1.0f);
    float dot = a.dot(b);
    assert(std::abs(dot - (-2.0f)) < 0.0001f);
    return true;
}

TEST(Vector3, Length)
{
    engine::math::Vector3 v(2.0f, 3.0f, 6.0f);
    float len = v.length();
    assert(std::abs(len - 7.0f) < 0.0001f);
    return true;
}

TEST(Vector3, Normalize)
{
    engine::math::Vector3 v(0.0f, 0.0f, 10.0f);
    engine::math::Vector3 n = v.normalized();
    assert(std::abs(n.x - 0.0f) < 0.0001f);
    assert(std::abs(n.y - 0.0f) < 0.0001f);
    assert(std::abs(n.z - 1.0f) < 0.0001f);
    return true;
}

TEST(Vector3, Addition)
{
    engine::math::Vector3 a(0.5f, 1.5f, 2.5f);
    engine::math::Vector3 b(1.0f, 1.0f, 1.0f);
    auto c = a + b;
    assert(std::abs(c.x - 1.5f) < 0.0001f);
    assert(std::abs(c.y - 2.5f) < 0.0001f);
    assert(std::abs(c.z - 3.5f) < 0.0001f);
    return true;
}

TEST(Vector3, ScalarMultiplication)
{
    engine::math::Vector3 v(1.0f, -2.0f, 3.0f);
    auto r = v * 3.0f;
    assert(r.x == 3.0f);
    assert(r.y == -6.0f);
    assert(r.z == 9.0f);
    return true;
}

TEST(Vector3, Distance)
{
    engine::math::Vector3 a(0.0f, 0.0f, 0.0f);
    engine::math::Vector3 b(3.0f, 4.0f, 0.0f);
    float d = a.distance(b);
    assert(std::abs(d - 5.0f) < 0.0001f);
    return true;
}

}
