#include "EntityManager.h"
#include <cassert>

namespace engine {
namespace ecs {

EntityManager::EntityManager()
{
    m_entities.resize(MAX_ENTITIES);
}

Entity EntityManager::CreateEntity()
{
    uint32_t index = AllocateIndex();
    if (index >= MAX_ENTITIES)
        return Entity::Null();

    EntityInfo& info = m_entities[index];
    info.active = true;
    info.mask.Clear();

    EntityVersion version = info.version;
    EntityID id = Entity::FromID(
        (static_cast<EntityID>(version) << ENTITY_INDEX_BITS) | index).GetID();

    m_activeCount++;
    return Entity(id);
}

bool EntityManager::DestroyEntity(Entity entity)
{
    if (entity.IsNull()) return false;

    uint32_t index = entity.GetIndex();
    if (index >= MAX_ENTITIES) return false;

    EntityInfo& info = m_entities[index];
    if (!info.active) return false;

    info.active = false;
    info.mask.Clear();
    info.version++;

    FreeIndex(index);
    m_activeCount--;
    return true;
}

bool EntityManager::IsAlive(Entity entity) const
{
    if (entity.IsNull()) return false;
    uint32_t index = entity.GetIndex();
    if (index >= MAX_ENTITIES) return false;

    const EntityInfo& info = m_entities[index];
    return info.active && info.version == entity.GetVersion();
}

EntityVersion EntityManager::GetVersion(Entity entity) const
{
    if (entity.IsNull()) return 0;
    uint32_t index = entity.GetIndex();
    if (index >= MAX_ENTITIES) return 0;
    return m_entities[index].version;
}

void EntityManager::SetComponentMask(Entity entity, const ComponentMask& mask)
{
    if (!IsAlive(entity)) return;
    m_entities[entity.GetIndex()].mask = mask;
}

const ComponentMask& EntityManager::GetComponentMask(Entity entity) const
{
    static ComponentMask nullMask;
    if (!IsAlive(entity)) return nullMask;
    return m_entities[entity.GetIndex()].mask;
}

void EntityManager::AddComponentType(Entity entity, ComponentTypeID typeID)
{
    if (!IsAlive(entity)) return;
    m_entities[entity.GetIndex()].mask.SetBit(typeID, true);
}

void EntityManager::RemoveComponentType(Entity entity, ComponentTypeID typeID)
{
    if (!IsAlive(entity)) return;
    m_entities[entity.GetIndex()].mask.SetBit(typeID, false);
}

bool EntityManager::HasComponent(Entity entity, ComponentTypeID typeID) const
{
    if (!IsAlive(entity)) return false;
    return m_entities[entity.GetIndex()].mask.GetBit(typeID);
}

uint32_t EntityManager::AllocateIndex()
{
    if (!m_freeIndices.empty())
    {
        uint32_t index = m_freeIndices.front();
        m_freeIndices.pop();
        return index;
    }

    if (m_nextIndex < MAX_ENTITIES)
        return m_nextIndex++;

    return static_cast<uint32_t>(-1);
}

void EntityManager::FreeIndex(uint32_t index)
{
    m_freeIndices.push(index);
}

void EntityManager::Clear()
{
    m_entities.clear();
    m_entities.resize(MAX_ENTITIES);
    while (!m_freeIndices.empty())
        m_freeIndices.pop();
    m_activeCount = 0;
    m_nextIndex = 0;
}

std::vector<Entity> EntityManager::GetActiveEntities() const
{
    std::vector<Entity> entities;
    entities.reserve(m_activeCount);

    for (uint32_t i = 0; i < m_nextIndex; ++i)
    {
        if (m_entities[i].active)
        {
            EntityID id = (static_cast<EntityID>(m_entities[i].version) << ENTITY_INDEX_BITS) | i;
            entities.emplace_back(id);
        }
    }

    return entities;
}

} // namespace ecs
} // namespace engine
// v2: Added debug logging
// v3: Fixed version overflow
// v4: Debug iterator validation
