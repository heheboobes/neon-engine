#include "../main.cpp"
#include <engine/math/MathUtils.h>
#include <cmath>
#include <cassert>
#include <algorithm>

namespace tests::math
{

TEST(MathUtils, Clamp)
{
    assert(engine::math::clamp(5, 0, 10) == 5);
    assert(engine::math::clamp(-5, 0, 10) == 0);
    assert(engine::math::clamp(15, 0, 10) == 10);
    return true;
}

TEST(MathUtils, ClampFloat)
{
    float v = engine::math::clamp(3.5f, 0.0f, 1.0f);
    assert(std::abs(v - 1.0f) < 0.0001f);
    v = engine::math::clamp(-0.5f, 0.0f, 1.0f);
    assert(std::abs(v - 0.0f) < 0.0001f);
    v = engine::math::clamp(0.5f, 0.0f, 1.0f);
    assert(std::abs(v - 0.5f) < 0.0001f);
    return true;
}

TEST(MathUtils, Lerp)
{
    float v = engine::math::lerp(0.0f, 10.0f, 0.5f);
    assert(std::abs(v - 5.0f) < 0.0001f);
    v = engine::math::lerp(0.0f, 10.0f, 0.0f);
    assert(std::abs(v - 0.0f) < 0.0001f);
    v = engine::math::lerp(0.0f, 10.0f, 1.0f);
    assert(std::abs(v - 10.0f) < 0.0001f);
    return true;
}

TEST(MathUtils, DegreesToRadians)
{
    float r = engine::math::toRadians(180.0f);
    assert(std::abs(r - 3.14159265f) < 0.0001f);
    r = engine::math::toRadians(0.0f);
    assert(std::abs(r) < 0.0001f);
    r = engine::math::toRadians(90.0f);
    assert(std::abs(r - 1.57079633f) < 0.0001f);
    return true;
}

TEST(MathUtils, RadiansToDegrees)
{
    float d = engine::math::toDegrees(3.14159265f);
    assert(std::abs(d - 180.0f) < 0.0001f);
    d = engine::math::toDegrees(0.0f);
    assert(std::abs(d) < 0.0001f);
    return true;
}

TEST(MathUtils, SmoothStep)
{
    float v = engine::math::smoothStep(0.0f);
    assert(std::abs(v - 0.0f) < 0.0001f);
    v = engine::math::smoothStep(1.0f);
    assert(std::abs(v - 1.0f) < 0.0001f);
    v = engine::math::smoothStep(0.5f);
    assert(std::abs(v - 0.5f) < 0.0001f);
    return true;
}

TEST(MathUtils, IsNearlyEqual)
{
    assert(engine::math::isNearlyEqual(1.0f, 1.000001f));
    assert(!engine::math::isNearlyEqual(1.0f, 1.1f));
    assert(engine::math::isNearlyEqual(0.0f, 0.0f));
    return true;
}

TEST(MathUtils, MinMax)
{
    assert(engine::math::min(3, 7) == 3);
    assert(engine::math::max(3, 7) == 7);
    assert(engine::math::min(5.0f, 2.0f) == 2.0f);
    assert(engine::math::max(5.0f, 2.0f) == 5.0f);
    return true;
}

TEST(MathUtils, Sign)
{
    assert(engine::math::sign(5) == 1);
    assert(engine::math::sign(-3) == -1);
    assert(engine::math::sign(0) == 0);
    assert(engine::math::signf(2.5f) == 1.0f);
    assert(engine::math::signf(-4.2f) == -1.0f);
    return true;
}

TEST(MathUtils, Wrap)
{
    assert(engine::math::wrap(5, 0, 10) == 5);
    assert(engine::math::wrap(12, 0, 10) == 2);
    assert(engine::math::wrap(-3, 0, 10) == 7);
    return true;
}

}
