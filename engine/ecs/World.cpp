#include "World.h"
#include <cassert>

namespace engine {
namespace ecs {

World::World()
{
}

World::~World()
{
    Clear();
}

Entity World::CreateEntity()
{
    Entity entity = m_entityManager.CreateEntity();
    if (entity.IsNull())
        return Entity::Null();

    ComponentMask emptyMask;
    Archetype* archetype = GetOrCreateArchetype(emptyMask);
    if (archetype)
    {
        archetype->AddEntity(entity);
        m_entityArchetypeMap[entity.GetID()] = archetype;
    }

    for (auto& system : m_systems)
        system->OnEntityCreated(this, entity);

    return entity;
}

bool World::DestroyEntity(Entity entity)
{
    if (!m_entityManager.IsAlive(entity))
        return false;

    for (auto& system : m_systems)
        system->OnEntityDestroyed(this, entity);

    Archetype* arch = GetArchetypeForEntity(entity);
    if (arch)
        arch->RemoveEntity(entity);

    m_entityArchetypeMap.erase(entity.GetID());
    return m_entityManager.DestroyEntity(entity);
}

bool World::IsAlive(Entity entity) const
{
    return m_entityManager.IsAlive(entity);
}

Archetype* World::GetOrCreateArchetype(const ComponentMask& mask)
{
    for (auto& arch : m_archetypes)
    {
        if (arch->GetMask() == mask)
            return arch.get();
    }

    auto componentTypes = std::vector<ComponentTypeID>();
    auto componentSizes = std::vector<size_t>();
    for (ComponentTypeID i = 0; i < MAX_COMPONENT_TYPES; ++i)
    {
        if (mask.GetBit(i))
        {
            componentTypes.push_back(i);
            const ComponentTypeInfo* info = ComponentRegistry::Instance().GetTypeInfo(i);
            componentSizes.push_back(info ? info->size : 0);
        }
    }

    auto archetype = std::make_unique<Archetype>(mask, componentTypes, componentSizes);
    Archetype* ptr = archetype.get();
    m_archetypes.push_back(std::move(archetype));
    return ptr;
}

Archetype* World::GetArchetypeForEntity(Entity entity)
{
    auto it = m_entityArchetypeMap.find(entity.GetID());
    if (it != m_entityArchetypeMap.end())
        return it->second;
    return nullptr;
}

const Archetype* World::GetArchetypeForEntity(Entity entity) const
{
    auto it = m_entityArchetypeMap.find(entity.GetID());
    if (it != m_entityArchetypeMap.end())
        return it->second;
    return nullptr;
}

void World::MoveEntityToCorrectArchetype(Entity entity)
{
    const ComponentMask& newMask = m_entityManager.GetComponentMask(entity);
    Archetype* newArch = GetOrCreateArchetype(newMask);
    Archetype* oldArch = GetArchetypeForEntity(entity);

    if (oldArch == newArch)
        return;

    if (oldArch)
    {
        Chunk* oldChunk = oldArch->GetChunkForEntity(entity);
        if (oldChunk)
        {
            size_t idx = oldChunk->GetEntityIndex(entity);

            Chunk* newChunk = newArch->FindOrCreateChunk();
            size_t newIdx = newChunk->GetEntityCount();
            newChunk->SetEntity(newIdx, entity);

            for (ComponentTypeID typeID : oldChunk->GetComponentTypes())
            {
                if (newMask.GetBit(typeID))
                {
                    void* srcData = oldChunk->GetComponentData(typeID);
                    void* dstData = newChunk->GetComponentData(typeID);
                    if (srcData && dstData)
                    {
                        const ComponentTypeInfo* info =
                            ComponentRegistry::Instance().GetTypeInfo(typeID);
                        if (info)
                        {
                            uint8_t* src = static_cast<uint8_t*>(srcData);
                            uint8_t* dst = static_cast<uint8_t*>(dstData);
                            std::memcpy(dst + newIdx * info->size,
                                       src + idx * info->size,
                                       info->size);
                        }
                    }
                }
            }

            newChunk->GetEntityCount();
            oldChunk->RemoveEntity(idx);

            if (oldArch->IsEmpty())
            {
                auto it = std::remove_if(m_archetypes.begin(), m_archetypes.end(),
                    [oldArch](const std::unique_ptr<Archetype>& a) {
                        return a.get() == oldArch;
                    });
                m_archetypes.erase(it, m_archetypes.end());
            }
        }
    }
    else
    {
        newArch->AddEntity(entity);
    }

    m_entityArchetypeMap[entity.GetID()] = newArch;
}

void World::Update(float deltaTime)
{
    for (auto& system : m_systems)
    {
        if (!system->IsEnabled())
            continue;

        system->OnUpdate(this, deltaTime);
    }
}

void World::Render(float alpha)
{
    for (auto& system : m_systems)
    {
        if (!system->IsEnabled())
            continue;

        system->OnRender(this, alpha);
    }
}

void World::SortSystems()
{
    std::sort(m_systems.begin(), m_systems.end(),
        [](const std::unique_ptr<ISystem>& a, const std::unique_ptr<ISystem>& b) {
            return a->GetPriority() < b->GetPriority();
        });
}

void World::Clear()
{
    for (auto& system : m_systems)
        system->OnDetach(this);

    m_systems.clear();
    m_archetypes.clear();
    m_entityArchetypeMap.clear();
    m_entityManager.Clear();
}

} // namespace ecs
} // namespace engine
// v2: Added clear and reset
// v3: Fixed system sort stability
