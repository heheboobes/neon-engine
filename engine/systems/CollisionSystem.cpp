#include "CollisionSystem.h"
#include "../ecs/World.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace engine {
namespace systems {

CollisionSystem::CollisionSystem()
    : m_broadPhaseEnabled(true)
    , m_debugDraw(false)
    , m_world(nullptr)
{
    SetName("CollisionSystem");
    SetPriority(60);
    Require<components::CollisionComponent, components::TransformComponent>();
}

void CollisionSystem::SetBroadPhaseEnabled(bool enabled)
{
    m_broadPhaseEnabled = enabled;
}

void CollisionSystem::SetDebugDraw(bool enabled)
{
    m_debugDraw = enabled;
}

void CollisionSystem::SetCollisionCallback(std::function<void(const CollisionEvent&)> callback)
{
    m_callbacks.push_back(std::move(callback));
}

void CollisionSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    (void)deltaTime;
    m_world = world;

    std::vector<CollisionPair> broadPairs;
    BroadPhase(world, broadPairs);

    std::vector<CollisionPair> narrowPairs;
    NarrowPhase(broadPairs, narrowPairs);

    for (auto& pair : narrowPairs)
    {
        auto* colA = world->GetComponent<components::CollisionComponent>(pair.entityA);
        auto* colB = world->GetComponent<components::CollisionComponent>(pair.entityB);
        if (colA && colB)
        {
            if (!pair.isTrigger)
            {
                ResolveCollision(*colA, *colB, pair);
            }
        }
    }

    UpdateCollisionCache(narrowPairs);
    DispatchEvents(narrowPairs);
}

void CollisionSystem::OnRender(ecs::World* world, float alpha)
{
    (void)world;
    (void)alpha;

    if (!m_debugDraw)
        return;
}

void CollisionSystem::BroadPhase(ecs::World* world, std::vector<CollisionPair>& outPairs)
{
    outPairs.clear();

    std::vector<std::pair<ecs::Entity, components::CollisionComponent*>> candidates;
    std::unordered_map<ecs::Entity, components::TransformComponent*> transformMap;

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* cols = static_cast<components::CollisionComponent*>(
                chunk.GetComponentData(components::CollisionComponent::GetStaticTypeID()));
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!cols || !transforms)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                if (!cols[i].enabled)
                    continue;
                candidates.emplace_back(chunk.GetEntity(i), &cols[i]);
                transformMap[chunk.GetEntity(i)] = &transforms[i];
            }
        }
    }

    for (size_t i = 0; i < candidates.size(); ++i)
    {
        for (size_t j = i + 1; j < candidates.size(); ++j)
        {
            auto& a = *candidates[i].second;
            auto& b = *candidates[j].second;

            if (!a.CanCollideWith(b.layer))
                continue;
            if (!b.CanCollideWith(a.layer))
                continue;

            ecs::Entity entityA = candidates[i].first;
            ecs::Entity entityB = candidates[j].first;

            sf::Vector2f posA = transformMap[entityA]->position;
            sf::Vector2f posB = transformMap[entityB]->position;

            sf::FloatRect aabbA = a.GetTransformedAABB(posA);
            sf::FloatRect aabbB = b.GetTransformedAABB(posB);

            if (aabbA.intersects(aabbB))
            {
                CollisionPair pair;
                pair.entityA = entityA;
                pair.entityB = entityB;
                pair.isTrigger = a.isTrigger || b.isTrigger;
                outPairs.push_back(pair);
            }
        }
    }
}

void CollisionSystem::NarrowPhase(const std::vector<CollisionPair>& broad,
                                   std::vector<CollisionPair>& outPairs)
{
    outPairs.clear();

    for (const auto& pair : broad)
    {
        auto* colA = m_world->GetComponent<components::CollisionComponent>(pair.entityA);
        auto* colB = m_world->GetComponent<components::CollisionComponent>(pair.entityB);
        auto* transA = m_world->GetComponent<components::TransformComponent>(pair.entityA);
        auto* transB = m_world->GetComponent<components::TransformComponent>(pair.entityB);

        if (!colA || !colB || !transA || !transB)
            continue;

        sf::Vector2f posA(transA->position.x, transA->position.y);
        sf::Vector2f posB(transB->position.x, transB->position.y);

        CollisionPair result = pair;

        if (colA->shapeType == components::CollisionShapeType::AABB &&
            colB->shapeType == components::CollisionShapeType::AABB)
        {
            sf::Vector2f overlap, normal;
            float penetration;
            if (TestAABBvsAABB(*colA, *colB, posA, posB, overlap, normal, penetration))
            {
                result.overlap = overlap;
                result.normal = normal;
                result.penetration = penetration;
                outPairs.push_back(result);
            }
        }
    }
}

bool CollisionSystem::TestAABBvsAABB(const components::CollisionComponent& a,
                                      const components::CollisionComponent& b,
                                      const sf::Vector2f& posA,
                                      const sf::Vector2f& posB,
                                      sf::Vector2f& outOverlap,
                                      sf::Vector2f& outNormal,
                                      float& outPenetration)
{
    sf::FloatRect rectA = a.GetTransformedAABB(posA);
    sf::FloatRect rectB = b.GetTransformedAABB(posB);

    sf::FloatRect intersection;
    if (!rectA.intersects(rectB, intersection))
        return false;

    float overlapX = intersection.width;
    float overlapY = intersection.height;

    outOverlap = sf::Vector2f(overlapX, overlapY);

    float dx = rectB.left + rectB.width / 2.0f - (rectA.left + rectA.width / 2.0f);
    float dy = rectB.top + rectB.height / 2.0f - (rectA.top + rectA.height / 2.0f);

    outPenetration = std::min(overlapX, overlapY);

    if (overlapX < overlapY)
    {
        outNormal = sf::Vector2f(dx > 0.0f ? 1.0f : -1.0f, 0.0f);
    }
    else
    {
        outNormal = sf::Vector2f(0.0f, dy > 0.0f ? 1.0f : -1.0f);
    }

    return true;
}

bool CollisionSystem::TestCirclevsAABB(const components::CollisionComponent& circle,
                                        const components::CollisionComponent& aabb,
                                        const sf::Vector2f& posCircle,
                                        const sf::Vector2f& posAABB)
{
    (void)circle;
    (void)aabb;
    (void)posCircle;
    (void)posAABB;
    return false;
}

void CollisionSystem::ResolveCollision(components::CollisionComponent& a,
                                        components::CollisionComponent& b,
                                        const CollisionPair& pair)
{
    auto* phyA = m_world->GetComponent<components::PhysicsComponent>(pair.entityA);
    auto* phyB = m_world->GetComponent<components::PhysicsComponent>(pair.entityB);
    auto* transA = m_world->GetComponent<components::TransformComponent>(pair.entityA);
    auto* transB = m_world->GetComponent<components::TransformComponent>(pair.entityB);

    if (!transA || !transB)
        return;

    if (phyA && !phyA->kinematic && !phyA->frozen)
    {
        transA->position.x -= pair.normal.x * pair.penetration * 0.5f;
        transA->position.y -= pair.normal.y * pair.penetration * 0.5f;
        transA->MarkDirty();

        float combinedRestitution = (a.bounciness + b.bounciness) * 0.5f;
        float velDotNormal = phyA->velocity.x * pair.normal.x + phyA->velocity.y * pair.normal.y;

        if (velDotNormal < 0.0f)
        {
            phyA->velocity -= pair.normal * velDotNormal * (1.0f + combinedRestitution);
            phyA->velocity *= (1.0f - a.friction * 0.1f);
        }
    }

    if (phyB && !phyB->kinematic && !phyB->frozen)
    {
        transB->position.x += pair.normal.x * pair.penetration * 0.5f;
        transB->position.y += pair.normal.y * pair.penetration * 0.5f;
        transB->MarkDirty();
    }
}

void CollisionSystem::UpdateCollisionCache(const std::vector<CollisionPair>& currentPairs)
{
    for (auto& entry : m_collisionCache)
    {
        entry.second.wasColliding = false;
    }

    for (const auto& pair : currentPairs)
    {
        CollisionKey key = MakeKey(pair.entityA, pair.entityB);
        auto it = m_collisionCache.find(key);
        if (it == m_collisionCache.end())
        {
            CollisionCacheEntry entry;
            entry.wasColliding = false;
            entry.time = 0.0f;
            m_collisionCache[key] = entry;
        }
        else
        {
            it->second.wasColliding = true;
        }
    }
}

void CollisionSystem::DispatchEvents(const std::vector<CollisionPair>& currentPairs)
{
    for (const auto& pair : currentPairs)
    {
        CollisionKey key = MakeKey(pair.entityA, pair.entityB);
        auto it = m_collisionCache.find(key);
        CollisionEvent event;

        if (it != m_collisionCache.end() && !it->second.wasColliding)
        {
            event.type = CollisionEvent::Enter;
        }
        else
        {
            event.type = CollisionEvent::Stay;
        }

        event.pair = pair;

        for (auto& callback : m_callbacks)
        {
            callback(event);
        }
    }
}

CollisionSystem::CollisionKey CollisionSystem::MakeKey(ecs::Entity a, ecs::Entity b) const
{
    uint64_t min = std::min(a.GetID(), b.GetID());
    uint64_t max = std::max(a.GetID(), b.GetID());
    return (min << 32) | max;
}

void CollisionSystem::DebugDrawPair(const CollisionPair& pair)
{
    (void)pair;
}

} // namespace systems
} // namespace engine
// v2: Spatial query optimization
// v3: Ghost collision fix
