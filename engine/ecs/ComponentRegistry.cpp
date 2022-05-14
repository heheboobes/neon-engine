#include "ComponentRegistry.h"
#include <cassert>

namespace engine {
namespace ecs {

ComponentRegistry& ComponentRegistry::Instance()
{
    static ComponentRegistry instance;
    return instance;
}

ComponentTypeID ComponentRegistry::GetTypeID(const std::string& name) const
{
    auto it = m_typeNames.find(name);
    if (it != m_typeNames.end())
        return it->second;
    return INVALID_COMPONENT_TYPE;
}

const ComponentTypeInfo* ComponentRegistry::GetTypeInfo(ComponentTypeID id) const
{
    auto it = m_typeInfos.find(id);
    if (it != m_typeInfos.end())
        return &it->second;
    return nullptr;
}

std::string ComponentRegistry::GetTypeName(ComponentTypeID id) const
{
    auto it = m_typeInfos.find(id);
    if (it != m_typeInfos.end())
        return it->second.name;
    return "UnknownComponent";
}

std::unique_ptr<IComponent> ComponentRegistry::CreateComponent(ComponentTypeID id) const
{
    auto it = m_createFunctions.find(id);
    if (it != m_createFunctions.end())
        return it->second();
    return nullptr;
}

bool ComponentRegistry::IsRegistered(ComponentTypeID id) const
{
    return m_typeInfos.find(id) != m_typeInfos.end();
}

bool ComponentRegistry::IsRegistered(const std::string& name) const
{
    return m_typeNames.find(name) != m_typeNames.end();
}

std::vector<ComponentTypeID> ComponentRegistry::GetAllRegisteredTypes() const
{
    std::vector<ComponentTypeID> types;
    types.reserve(m_typeInfos.size());
    for (const auto& pair : m_typeInfos)
        types.push_back(pair.first);
    return types;
}

} // namespace ecs
} // namespace engine

