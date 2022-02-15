#include "Vector2.h"

namespace engine { namespace math {

template class Vector2<float>;
template class Vector2<int>;
template class Vector2<double>;
template Vector2<float> operator*(float, const Vector2<float>&);
template Vector2<double> operator*(double, const Vector2<double>&);

}} // namespace engine::math
// v2: Fixed normalization edge case
