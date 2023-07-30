#ifndef ENGINE_ECS_SYSTEM_H
#define ENGINE_ECS_SYSTEM_H

#include "Entity.h"
#include "ComponentMask.h"
#include <string>
#include <cstdint>
#include <vector>

namespace engine {
namespace ecs {

class World;

class ISystem {
public:
    ISystem() = default;
    virtual ~ISystem() = default;

    virtual void OnAttach(World* world) { (void)world; }
    virtual void OnDetach(World* world) { (void)world; }
    virtual void OnUpdate(World* world, float deltaTime) = 0;
    virtual void OnRender(World* world, float alpha) { (void)world; (void)alpha; }
    virtual void OnEntityCreated(World* world, Entity entity) { (void)world; (void)entity; }
    virtual void OnEntityDestroyed(World* world, Entity entity) { (void)world; (void)entity; }

    void SetComponentMask(const ComponentMask& mask) { m_componentMask = mask; }
    const ComponentMask& GetComponentMask() const { return m_componentMask; }

    void SetName(const std::string& name) { m_name = name; }
    const std::string& GetName() const { return m_name; }

    void SetPriority(int32_t priority) { m_priority = priority; }
    int32_t GetPriority() const { return m_priority; }

    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }

    template<typename... Components>
    void Require()
    {
        (m_componentMask.SetBit(Components::GetStaticTypeID()), ...);
    }

private:
    ComponentMask m_componentMask;
    std::string m_name = "System";
    int32_t m_priority = 0;
    bool m_enabled = true;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_SYSTEM_H
// v2: Added nodiscard
