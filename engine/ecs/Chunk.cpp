#include "Chunk.h"
#include <cstdlib>
#include <algorithm>
#include <cassert>

namespace engine {
namespace ecs {

Chunk::Chunk()
{
    m_entities.resize(CHUNK_CAPACITY);
}

Chunk::~Chunk()
{
    Free();
}

Chunk::Chunk(Chunk&& other) noexcept
    : m_componentArrays(std::move(other.m_componentArrays))
    , m_entities(std::move(other.m_entities))
    , m_entityCount(other.m_entityCount)
{
    other.m_entityCount = 0;
}

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
    if (this != &other)
    {
        Free();
        m_componentArrays = std::move(other.m_componentArrays);
        m_entities = std::move(other.m_entities);
        m_entityCount = other.m_entityCount;
        other.m_entityCount = 0;
    }
    return *this;
}

void Chunk::AddComponent(ComponentTypeID typeID, size_t componentSize)
{
    for (auto& arr : m_componentArrays)
    {
        if (arr.typeID == typeID)
            return;
    }

    ComponentArray newArray;
    newArray.typeID = typeID;
    newArray.elementSize = componentSize;
    newArray.data = std::malloc(CHUNK_CAPACITY * componentSize);
    if (newArray.data)
        std::memset(newArray.data, 0, CHUNK_CAPACITY * componentSize);

    m_componentArrays.push_back(newArray);
}

bool Chunk::HasComponentType(ComponentTypeID typeID) const
{
    for (const auto& arr : m_componentArrays)
    {
        if (arr.typeID == typeID)
            return true;
    }
    return false;
}

void* Chunk::GetComponentData(ComponentTypeID typeID)
{
    for (auto& arr : m_componentArrays)
    {
        if (arr.typeID == typeID)
            return arr.data;
    }
    return nullptr;
}

const void* Chunk::GetComponentData(ComponentTypeID typeID) const
{
    for (const auto& arr : m_componentArrays)
    {
        if (arr.typeID == typeID)
            return arr.data;
    }
    return nullptr;
}

Entity Chunk::GetEntity(size_t index) const
{
    if (index < m_entityCount)
        return m_entities[index];
    return Entity::Null();
}

void Chunk::SetEntity(size_t index, Entity entity)
{
    if (index < CHUNK_CAPACITY)
    {
        if (index >= m_entityCount)
            m_entityCount = index + 1;
        m_entities[index] = entity;
    }
}

size_t Chunk::GetEntityIndex(Entity entity) const
{
    for (size_t i = 0; i < m_entityCount; ++i)
    {
        if (m_entities[i] == entity)
            return i;
    }
    return static_cast<size_t>(-1);
}

bool Chunk::RemoveEntity(size_t index)
{
    if (index >= m_entityCount || m_entityCount == 0)
        return false;

    size_t lastIndex = m_entityCount - 1;

    m_entities[index] = m_entities[lastIndex];
    m_entities[lastIndex] = Entity::Null();

    for (auto& arr : m_componentArrays)
    {
        if (arr.data && arr.elementSize > 0)
        {
            uint8_t* bytes = static_cast<uint8_t*>(arr.data);
            std::memcpy(bytes + index * arr.elementSize,
                       bytes + lastIndex * arr.elementSize,
                       arr.elementSize);
            std::memset(bytes + lastIndex * arr.elementSize, 0, arr.elementSize);
        }
    }

    m_entityCount--;
    return true;
}

size_t Chunk::MoveEntityTo(Chunk& dest, size_t index)
{
    if (index >= m_entityCount) return static_cast<size_t>(-1);

    size_t destIndex = dest.m_entityCount;
    dest.m_entities[destIndex] = m_entities[index];

    for (auto& srcArr : m_componentArrays)
    {
        ComponentArray* destArr = nullptr;
        for (auto& da : dest.m_componentArrays)
        {
            if (da.typeID == srcArr.typeID)
            {
                destArr = &da;
                break;
            }
        }

        if (destArr && destArr->data && srcArr.data)
        {
            uint8_t* srcBytes = static_cast<uint8_t*>(srcArr.data);
            uint8_t* dstBytes = static_cast<uint8_t*>(destArr->data);
            std::memcpy(dstBytes + destIndex * destArr->elementSize,
                       srcBytes + index * srcArr.elementSize,
                       srcArr.elementSize);
        }
    }

    dest.m_entityCount++;
    RemoveEntity(index);
    return destIndex;
}

void Chunk::Clear()
{
    for (auto& arr : m_componentArrays)
    {
        if (arr.data)
            std::memset(arr.data, 0, CHUNK_CAPACITY * arr.elementSize);
    }
    std::fill(m_entities.begin(), m_entities.end(), Entity::Null());
    m_entityCount = 0;
}

void Chunk::Free()
{
    for (auto& arr : m_componentArrays)
    {
        if (arr.data)
        {
            std::free(arr.data);
            arr.data = nullptr;
        }
        arr.elementSize = 0;
    }
    m_componentArrays.clear();
    m_entities.clear();
    m_entityCount = 0;
}

std::vector<ComponentTypeID> Chunk::GetComponentTypes() const
{
    std::vector<ComponentTypeID> types;
    types.reserve(m_componentArrays.size());
    for (const auto& arr : m_componentArrays)
        types.push_back(arr.typeID);
    return types;
}

} // namespace ecs
} // namespace engine
