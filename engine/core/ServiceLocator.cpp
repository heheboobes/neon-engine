#include "ServiceLocator.h"

namespace engine {
namespace core {

ServiceLocator& ServiceLocator::GetInstance()
{
    static ServiceLocator instance;
    return instance;
}

void ServiceLocator::ClearAll()
{
    GetInstance().m_services.clear();
}

size_t ServiceLocator::GetServiceCount()
{
    return GetInstance().m_services.size();
}

} // namespace core
} // namespace engine
