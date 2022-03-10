#ifndef ENGINE_ECS_ARCHETYPE_H
#define ENGINE_ECS_ARCHETYPE_H

#include "Chunk.h"
#include "ComponentMask.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace engine {
namespace ecs {

class Archetype {
public:
    Archetype() = default;
    Archetype(const ComponentMask& mask,
              const std::vector<ComponentTypeID>& componentTypes,
              const std::vector<size_t>& componentSizes);
    ~Archetype() = default;

    Archetype(const Archetype&) = delete;
    Archetype& operator=(const Archetype&) = delete;
    Archetype(Archetype&&) noexcept = default;
    Archetype& operator=(Archetype&&) noexcept = default;

    const ComponentMask& GetMask() const { return m_mask; }
    const std::vector<ComponentTypeID>& GetComponentTypes() const { return m_componentTypes; }
    size_t GetEntityCount() const;
    size_t GetChunkCount() const { return m_chunks.size(); }
    bool IsEmpty() const { return GetEntityCount() == 0; }

    Chunk* GetChunk(size_t index);
    const Chunk* GetChunk(size_t index) const;

    Chunk* FindOrCreateChunk();
    Chunk* GetChunkForEntity(Entity entity);

    void AddEntity(Entity entity);
    bool RemoveEntity(Entity entity);
    bool HasEntity(Entity entity) const;
    size_t GetEntityIndex(Entity entity) const;

    template<typename T>
    T* GetComponentArray(Chunk* chunk)
    {
        return chunk->GetComponentArray<T>();
    }

    template<typename T>
    const T* GetComponentArray(const Chunk* chunk) const
    {
        return chunk->GetComponentArray<T>();
    }

    void Clear();

private:
    ComponentMask m_mask;
    std::vector<ComponentTypeID> m_componentTypes;
    std::vector<size_t> m_componentSizes;
    std::vector<std::unique_ptr<Chunk>> m_chunks;
};

} // namespace ecs
} // namespace engine

#endif // ENGINE_ECS_ARCHETYPE_H
