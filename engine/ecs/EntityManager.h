#ifndef ENGINE_ECS_ENTITY_MANAGER_H
#define ENGINE_ECS_ENTITY_MANAGER_H

#include "Entity.h"
#include "ComponentMask.h"
#include <vector>
#include <queue>
#include <cstdint>

namespace engine {
namespace ecs {

struct EntityInfo {
    EntityVersion version = 0;
    ComponentMask mask;
    bool active = false;
};

class EntityManager {
public:
    EntityManager();
    ~EntityManager() = default;

    Entity CreateEntity();
    bool DestroyEntity(Entity entity);
    bool IsAlive(Entity entity) const;

    EntityVersion GetVersion(Entity entity) const;
    void SetComponentMask(Entity entity, const ComponentMask& mask);
    const ComponentMask& GetComponentMask(Entity entity) const;

    void AddComponentType(Entity entity, ComponentTypeID typeID);
    void RemoveComponentType(Entity entity, ComponentTypeID typeID);
    bool HasComponent(Entity entity, ComponentTypeID typeID) const;

    template<typename T>
    bool HasComponent(Entity entity) const
    {
        return HasComponent(entity, T::GetStaticTypeID());
    }

    size_t GetEntityCount() const { return m_activeCount; }
    size_t GetCapacity() const { return MAX_ENTITIES; }
    bool IsFull() const { return m_activeCount >= MAX_ENTITIES; }

    void Clear();
    std::vector<Entity> GetActiveEntities() const;

private:
    uint32_t AllocateIndex();
    void FreeIndex(uint32_t index);

    std::vector<EntityInfo> m_entities;
    std::queue<uint32_t> m_freeIndices;
    size_t m_activeCount = 0;
    uint32_t m_nextIndex = 0;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_ENTITY_MANAGER_H
