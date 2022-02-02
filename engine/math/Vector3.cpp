#include "Vector3.h"

namespace engine { namespace math {

template class Vector3<float>;
template class Vector3<int>;
template class Vector3<double>;
template Vector3<float> operator*(float, const Vector3<float>&);
template Vector3<double> operator*(double, const Vector3<double>&);

}} // namespace engine::math
