#ifndef ENGINE_ECS_COMPONENT_H
#define ENGINE_ECS_COMPONENT_H

#include <cstdint>
#include <typeindex>
#include <typeinfo>
#include <memory>

namespace engine {
namespace ecs {

using ComponentTypeID = uint32_t;
constexpr ComponentTypeID MAX_COMPONENT_TYPES = 64;
constexpr ComponentTypeID INVALID_COMPONENT_TYPE = ~0u;

class IComponent {
public:
    virtual ~IComponent() = default;
    virtual ComponentTypeID GetTypeID() const = 0;
    virtual std::unique_ptr<IComponent> Clone() const = 0;
};

template<typename T>
class Component : public IComponent {
public:
    static ComponentTypeID GetStaticTypeID()
    {
        static const ComponentTypeID typeID = s_nextTypeID++;
        return typeID;
    }

    ComponentTypeID GetTypeID() const override
    {
        return GetStaticTypeID();
    }

    std::unique_ptr<IComponent> Clone() const override
    {
        return std::make_unique<T>(static_cast<const T&>(*this));
    }

    static bool IsValid()
    {
        return GetStaticTypeID() < MAX_COMPONENT_TYPES;
    }

private:
    static ComponentTypeID s_nextTypeID;
};

template<typename T>
ComponentTypeID Component<T>::s_nextTypeID = 0;

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_COMPONENT_H
