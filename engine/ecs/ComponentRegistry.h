#ifndef ENGINE_ECS_COMPONENT_REGISTRY_H
#define ENGINE_ECS_COMPONENT_REGISTRY_H

#include "Component.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace engine {
namespace ecs {

struct ComponentTypeInfo {
    ComponentTypeID id = INVALID_COMPONENT_TYPE;
    std::string name;
    size_t size = 0;
    size_t alignment = 0;
};

class ComponentRegistry {
public:
    static ComponentRegistry& Instance();

    template<typename T>
    ComponentTypeID Register(const std::string& name = "")
    {
        ComponentTypeID id = Component<T>::GetStaticTypeID();
        std::string typeName = name.empty() ? typeid(T).name() : name;

        ComponentTypeInfo info;
        info.id = id;
        info.name = typeName;
        info.size = sizeof(T);
        info.alignment = alignof(T);

        m_typeInfos[id] = info;
        m_typeNames[typeName] = id;

        m_createFunctions[id] = []() -> std::unique_ptr<IComponent> {
            return std::make_unique<T>();
        };

        return id;
    }

    ComponentTypeID GetTypeID(const std::string& name) const;
    const ComponentTypeInfo* GetTypeInfo(ComponentTypeID id) const;
    std::string GetTypeName(ComponentTypeID id) const;

    std::unique_ptr<IComponent> CreateComponent(ComponentTypeID id) const;
    size_t GetTypeCount() const { return m_typeInfos.size(); }

    bool IsRegistered(ComponentTypeID id) const;
    bool IsRegistered(const std::string& name) const;

    std::vector<ComponentTypeID> GetAllRegisteredTypes() const;

private:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;

    std::unordered_map<ComponentTypeID, ComponentTypeInfo> m_typeInfos;
    std::unordered_map<std::string, ComponentTypeID> m_typeNames;
    std::unordered_map<ComponentTypeID, std::function<std::unique_ptr<IComponent>()>> m_createFunctions;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_COMPONENT_REGISTRY_H
