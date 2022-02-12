#include "../main.cpp"
#include <engine/math/Vector2.h>
#include <cmath>
#include <cassert>

namespace tests::math
{

TEST(Vector2, DefaultConstructor)
{
    engine::math::Vector2 v;
    assert(v.x == 0.0f);
    assert(v.y == 0.0f);
    return true;
}

TEST(Vector2, ParameterizedConstructor)
{
    engine::math::Vector2 v(3.0f, 4.0f);
    assert(v.x == 3.0f);
    assert(v.y == 4.0f);
    return true;
}

TEST(Vector2, Addition)
{
    engine::math::Vector2 a(1.0f, 2.0f);
    engine::math::Vector2 b(3.0f, 4.0f);
    engine::math::Vector2 c = a + b;
    assert(c.x == 4.0f);
    assert(c.y == 6.0f);
    return true;
}

TEST(Vector2, Subtraction)
{
    engine::math::Vector2 a(5.0f, 8.0f);
    engine::math::Vector2 b(2.0f, 3.0f);
    engine::math::Vector2 c = a - b;
    assert(c.x == 3.0f);
    assert(c.y == 5.0f);
    return true;
}

TEST(Vector2, ScalarMultiplication)
{
    engine::math::Vector2 v(2.0f, 3.0f);
    engine::math::Vector2 r = v * 4.0f;
    assert(r.x == 8.0f);
    assert(r.y == 12.0f);
    return true;
}

TEST(Vector2, DotProduct)
{
    engine::math::Vector2 a(1.0f, 0.0f);
    engine::math::Vector2 b(0.0f, 1.0f);
    float dot = a.dot(b);
    assert(dot == 0.0f);
    return true;
}

TEST(Vector2, Length)
{
    engine::math::Vector2 v(3.0f, 4.0f);
    float len = v.length();
    assert(std::abs(len - 5.0f) < 0.0001f);
    return true;
}

TEST(Vector2, Normalize)
{
    engine::math::Vector2 v(0.0f, 5.0f);
    engine::math::Vector2 n = v.normalized();
    assert(std::abs(n.x - 0.0f) < 0.0001f);
    assert(std::abs(n.y - 1.0f) < 0.0001f);
    return true;
}

TEST(Vector2, Equality)
{
    engine::math::Vector2 a(1.5f, 2.5f);
    engine::math::Vector2 b(1.5f, 2.5f);
    engine::math::Vector2 c(1.5f, 3.0f);
    assert(a == b);
    assert(!(a == c));
    return true;
}

TEST(Vector2, Negation)
{
    engine::math::Vector2 v(10.0f, -5.0f);
    engine::math::Vector2 n = -v;
    assert(n.x == -10.0f);
    assert(n.y == 5.0f);
    return true;
}

}
