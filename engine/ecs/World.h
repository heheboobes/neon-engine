#ifndef ENGINE_ECS_WORLD_H
#define ENGINE_ECS_WORLD_H

#include "Entity.h"
#include "EntityManager.h"
#include "Component.h"
#include "ComponentRegistry.h"
#include "Archetype.h"
#include "System.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>

namespace engine {
namespace ecs {

class World {
public:
    World();
    ~World();

    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) = delete;
    World& operator=(World&&) = delete;

    Entity CreateEntity();
    bool DestroyEntity(Entity entity);
    bool IsAlive(Entity entity) const;

    template<typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args)
    {
        ComponentTypeID typeID = Component<T>::GetStaticTypeID();
        ComponentRegistry::Instance().Register<T>();

        if (!m_entityManager.IsAlive(entity))
        {
            static T dummy;
            return dummy;
        }

        m_entityManager.AddComponentType(entity, typeID);
        MoveEntityToCorrectArchetype(entity);

        Archetype* arch = GetArchetypeForEntity(entity);
        if (arch)
        {
            Chunk* chunk = arch->GetChunkForEntity(entity);
            if (chunk)
            {
                T* arr = static_cast<T*>(chunk->GetComponentData(typeID));
                if (arr)
                {
                    size_t idx = chunk->GetEntityIndex(entity);
                    new (&arr[idx]) T(std::forward<Args>(args)...);
                    return arr[idx];
                }
            }
        }

        static T fallback;
        return fallback;
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        ComponentTypeID typeID = Component<T>::GetStaticTypeID();
        if (!m_entityManager.HasComponent(entity, typeID))
            return;

        Archetype* oldArch = GetArchetypeForEntity(entity);
        if (oldArch)
        {
            Chunk* chunk = oldArch->GetChunkForEntity(entity);
            if (chunk)
            {
                size_t idx = chunk->GetEntityIndex(entity);
                T* arr = static_cast<T*>(chunk->GetComponentData(typeID));
                if (arr)
                    arr[idx].~T();
            }
        }

        m_entityManager.RemoveComponentType(entity, typeID);
        MoveEntityToCorrectArchetype(entity);
    }

    template<typename T>
    T* GetComponent(Entity entity)
    {
        ComponentTypeID typeID = Component<T>::GetStaticTypeID();
        if (!m_entityManager.HasComponent(entity, typeID))
            return nullptr;

        Archetype* arch = GetArchetypeForEntity(entity);
        if (!arch) return nullptr;

        Chunk* chunk = arch->GetChunkForEntity(entity);
        if (!chunk) return nullptr;

        T* arr = static_cast<T*>(chunk->GetComponentData(typeID));
        if (!arr) return nullptr;

        size_t idx = chunk->GetEntityIndex(entity);
        return &arr[idx];
    }

    template<typename T>
    const T* GetComponent(Entity entity) const
    {
        return const_cast<World*>(this)->GetComponent<T>(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity) const
    {
        return m_entityManager.HasComponent(entity, Component<T>::GetStaticTypeID());
    }

    template<typename TSystem, typename... Args>
    TSystem& RegisterSystem(Args&&... args)
    {
        auto system = std::make_unique<TSystem>(std::forward<Args>(args)...);
        TSystem* ptr = system.get();
        ptr->OnAttach(this);
        m_systems.push_back(std::move(system));
        SortSystems();
        return *ptr;
    }

    void Update(float deltaTime);
    void Render(float alpha);

    EntityManager& GetEntityManager() { return m_entityManager; }
    const EntityManager& GetEntityManager() const { return m_entityManager; }

    Archetype* GetOrCreateArchetype(const ComponentMask& mask);
    Archetype* GetArchetypeForEntity(Entity entity);
    const Archetype* GetArchetypeForEntity(Entity entity) const;

    size_t GetArchetypeCount() const { return m_archetypes.size(); }
    size_t GetSystemCount() const { return m_systems.size(); }

    void Clear();

private:
    void MoveEntityToCorrectArchetype(Entity entity);
    void SortSystems();

    EntityManager m_entityManager;
    std::vector<std::unique_ptr<Archetype>> m_archetypes;
    std::vector<std::unique_ptr<ISystem>> m_systems;
    std::unordered_map<uint64_t, Archetype*> m_entityArchetypeMap;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_WORLD_H
// v2: Added system sorting by priority
