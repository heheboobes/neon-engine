#include "Entity.h"

namespace engine {
namespace ecs {

std::ostream& operator<<(std::ostream& os, const Entity& entity)
{
    os << "Entity{ID: " << entity.m_id
       << ", Index: " << entity.GetIndex()
       << ", Version: " << entity.GetVersion()
       << "}";
    return os;
}

} // namespace ecs
} // namespace engine

