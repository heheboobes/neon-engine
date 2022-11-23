#include "PoolAllocator.h"

namespace engine::utils {

template class PoolAllocator<int, 4>;
template class PoolAllocator<double, 8>;
template class PoolAllocator<void*, 8>;

} // namespace engine::utils
