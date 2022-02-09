#include "MathUtils.h"

namespace engine { namespace math {

template int clamp<int>(int, int, int);
template float clamp<float>(float, float, float);
template double clamp<double>(double, double, double);

template float lerp<float>(float, float, float);
template double lerp<double>(double, double, double);

template float smoothstep<float>(float, float, float);
template double smoothstep<double>(double, double, double);

template float smootherstep<float>(float, float, float);
template double smootherstep<double>(double, double, double);

template float mapRange<float>(float, float, float, float, float);
template double mapRange<double>(double, double, double, double, double);

template float mapRangeClamped<float>(float, float, float, float, float);
template double mapRangeClamped<double>(double, double, double, double, double);

template float radians<float>(float);
template double radians<double>(double);

template float degrees<float>(float);
template double degrees<double>(double);

template bool isNearlyEqual<float>(float, float, float);
template bool isNearlyEqual<double>(double, double, double);

}} // namespace engine::math
