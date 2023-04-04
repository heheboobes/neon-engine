#ifndef ENGINE_ECS_CHUNK_H
#define ENGINE_ECS_CHUNK_H

#include "Entity.h"
#include "Component.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>

namespace engine {
namespace ecs {

constexpr size_t CHUNK_CAPACITY = 64;
constexpr size_t CHUNK_SIZE_BYTES = 16 * 1024;

class Chunk {
public:
    Chunk();
    ~Chunk();

    Chunk(Chunk&& other) noexcept;
    Chunk& operator=(Chunk&& other) noexcept;

    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    void AddComponent(ComponentTypeID typeID, size_t componentSize);
    size_t GetEntityCount() const { return m_entityCount; }
    size_t GetCapacity() const { return CHUNK_CAPACITY; }
    bool IsFull() const { return m_entityCount >= CHUNK_CAPACITY; }
    bool IsEmpty() const { return m_entityCount == 0; }

    bool HasComponentType(ComponentTypeID typeID) const;
    size_t GetComponentTypeCount() const { return m_componentArrays.size(); }

    void* GetComponentData(ComponentTypeID typeID);
    const void* GetComponentData(ComponentTypeID typeID) const;

    template<typename T>
    T* GetComponentArray()
    {
        return static_cast<T*>(GetComponentData(T::GetStaticTypeID()));
    }

    template<typename T>
    const T* GetComponentArray() const
    {
        return static_cast<const T*>(GetComponentData(T::GetStaticTypeID()));
    }

    Entity GetEntity(size_t index) const;
    void SetEntity(size_t index, Entity entity);

    size_t GetEntityIndex(Entity entity) const;

    bool RemoveEntity(size_t index);
    size_t MoveEntityTo(Chunk& dest, size_t index);

    void Clear();
    void Free();

    std::vector<ComponentTypeID> GetComponentTypes() const;

private:
    struct ComponentArray {
        void* data = nullptr;
        size_t elementSize = 0;
        ComponentTypeID typeID = INVALID_COMPONENT_TYPE;
    };

    std::vector<ComponentArray> m_componentArrays;
    std::vector<Entity> m_entities;
    size_t m_entityCount = 0;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_CHUNK_H
// v2: Added MoveEntityTo helper
// v3: Improved memory layout
