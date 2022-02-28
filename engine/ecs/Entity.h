#ifndef ENGINE_ECS_ENTITY_H
#define ENGINE_ECS_ENTITY_H

#include <cstdint>
#include <functional>
#include <ostream>

namespace engine {
namespace ecs {

using EntityID = uint64_t;
using EntityVersion = uint32_t;

constexpr EntityID ENTITY_NULL = 0;
constexpr uint32_t ENTITY_INDEX_BITS = 24;
constexpr uint32_t ENTITY_VERSION_BITS = 8;
constexpr EntityID ENTITY_INDEX_MASK = (EntityID(1) << ENTITY_INDEX_BITS) - 1;
constexpr EntityID ENTITY_VERSION_MASK = (EntityID(1) << ENTITY_VERSION_BITS) - 1;
constexpr uint32_t MAX_ENTITIES = ENTITY_INDEX_MASK;

class Entity {
public:
    Entity() : m_id(ENTITY_NULL) {}
    explicit Entity(EntityID id) : m_id(id) {}

    EntityID GetID() const { return m_id; }
    uint32_t GetIndex() const { return static_cast<uint32_t>(m_id & ENTITY_INDEX_MASK); }
    EntityVersion GetVersion() const {
        return static_cast<EntityVersion>((m_id >> ENTITY_INDEX_BITS) & ENTITY_VERSION_MASK);
    }

    bool IsNull() const { return m_id == ENTITY_NULL; }
    bool IsValid() const { return m_id != ENTITY_NULL; }

    bool operator==(const Entity& other) const { return m_id == other.m_id; }
    bool operator!=(const Entity& other) const { return m_id != other.m_id; }
    bool operator<(const Entity& other) const { return m_id < other.m_id; }

    explicit operator bool() const { return IsValid(); }
    explicit operator EntityID() const { return m_id; }

    static Entity Null() { return Entity(); }
    static Entity FromID(EntityID id) { return Entity(id); }

    friend std::ostream& operator<<(std::ostream& os, const Entity& entity);

private:
    EntityID m_id;

    static EntityID Pack(uint32_t index, EntityVersion version) {
        return (static_cast<EntityID>(version) << ENTITY_INDEX_BITS) | index;
    }

    friend class EntityManager;
};

} // namespace ecs
} // namespace engine

namespace std {
template<>
struct hash<engine::ecs::Entity> {
    size_t operator()(const engine::ecs::Entity& entity) const {
        return hash<engine::ecs::EntityID>()(entity.GetID());
    }
};
} // namespace std

#endif // ENGINE_ECS_ENTITY_H
