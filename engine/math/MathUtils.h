#pragma once

#include <cmath>
#include <type_traits>
#include <algorithm>

namespace engine { namespace math {

namespace constants {
    constexpr double PI       = 3.14159265358979323846;
    constexpr double TWO_PI   = 6.28318530717958647692;
    constexpr double HALF_PI  = 1.57079632679489661923;
    constexpr double EPSILON  = 1e-8;
    constexpr double DEG2RAD = PI / 180.0;
    constexpr double RAD2DEG = 180.0 / PI;
}

template <typename T>
constexpr T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

template <typename T>
T lerp(T a, T b, T t) {
    return a + (b - a) * t;
}

template <typename T>
T smoothstep(T edge0, T edge1, T x) {
    T t = clamp<T>((x - edge0) / (edge1 - edge0), 0, 1);
    return t * t * (3 - 2 * t);
}

template <typename T>
T smootherstep(T edge0, T edge1, T x) {
    T t = clamp<T>((x - edge0) / (edge1 - edge0), 0, 1);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

template <typename T>
T mapRange(T value, T inMin, T inMax, T outMin, T outMax) {
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

template <typename T>
T mapRangeClamped(T value, T inMin, T inMax, T outMin, T outMax) {
    T result = mapRange(value, inMin, inMax, outMin, outMax);
    T lo = std::min(outMin, outMax);
    T hi = std::max(outMin, outMax);
    return clamp(result, lo, hi);
}

template <typename T>
T radians(T degrees) {
    return static_cast<T>(degrees * constants::DEG2RAD);
}

template <typename T>
T degrees(T radians) {
    return static_cast<T>(radians * constants::RAD2DEG);
}

template <typename T>
bool isNearlyEqual(T a, T b, T epsilon = static_cast<T>(constants::EPSILON)) {
    return std::abs(a - b) <= epsilon;
}

template <typename T>
T wrap(T value, T min, T max) {
    T range = max - min;
    if (range == 0) return min;
    T result = value - range * std::floor((value - min) / range);
    return result;
}

template <typename T>
int sign(T value) {
    return (value > 0) - (value < 0);
}

template <typename T>
T sqr(T value) {
    return value * value;
}

}} // namespace engine::math
// v2: Added smoothstep and mapRange
