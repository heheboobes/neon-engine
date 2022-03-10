#include "Archetype.h"
#include <cassert>

namespace engine {
namespace ecs {

Archetype::Archetype(const ComponentMask& mask,
                     const std::vector<ComponentTypeID>& componentTypes,
                     const std::vector<size_t>& componentSizes)
    : m_mask(mask)
    , m_componentTypes(componentTypes)
    , m_componentSizes(componentSizes)
{
}

size_t Archetype::GetEntityCount() const
{
    size_t count = 0;
    for (const auto& chunk : m_chunks)
        count += chunk->GetEntityCount();
    return count;
}

Chunk* Archetype::GetChunk(size_t index)
{
    if (index < m_chunks.size())
        return m_chunks[index].get();
    return nullptr;
}

const Chunk* Archetype::GetChunk(size_t index) const
{
    if (index < m_chunks.size())
        return m_chunks[index].get();
    return nullptr;
}

Chunk* Archetype::FindOrCreateChunk()
{
    for (auto& chunk : m_chunks)
    {
        if (!chunk->IsFull())
            return chunk.get();
    }

    auto newChunk = std::make_unique<Chunk>();
    for (size_t i = 0; i < m_componentTypes.size(); ++i)
    {
        newChunk->AddComponent(m_componentTypes[i], m_componentSizes[i]);
    }

    Chunk* rawPtr = newChunk.get();
    m_chunks.push_back(std::move(newChunk));
    return rawPtr;
}

Chunk* Archetype::GetChunkForEntity(Entity entity)
{
    for (auto& chunk : m_chunks)
    {
        if (chunk->GetEntityIndex(entity) != static_cast<size_t>(-1))
            return chunk.get();
    }
    return nullptr;
}

void Archetype::AddEntity(Entity entity)
{
    Chunk* chunk = FindOrCreateChunk();
    assert(chunk != nullptr);
    chunk->SetEntity(chunk->GetEntityCount(), entity);
}

bool Archetype::RemoveEntity(Entity entity)
{
    for (auto& chunk : m_chunks)
    {
        size_t index = chunk->GetEntityIndex(entity);
        if (index != static_cast<size_t>(-1))
        {
            return chunk->RemoveEntity(index);
        }
    }
    return false;
}

bool Archetype::HasEntity(Entity entity) const
{
    for (const auto& chunk : m_chunks)
    {
        if (chunk->GetEntityIndex(entity) != static_cast<size_t>(-1))
            return true;
    }
    return false;
}

size_t Archetype::GetEntityIndex(Entity entity) const
{
    for (const auto& chunk : m_chunks)
    {
        size_t index = chunk->GetEntityIndex(entity);
        if (index != static_cast<size_t>(-1))
            return index;
    }
    return static_cast<size_t>(-1);
}

void Archetype::Clear()
{
    for (auto& chunk : m_chunks)
        chunk->Clear();
}

} // namespace ecs
} // namespace engine
