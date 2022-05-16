#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/CollisionComponent.h"
#include "../components/TransformComponent.h"
#include <vector>
#include <functional>
#include <cstdint>

namespace engine {
namespace systems {

struct CollisionPair {
    ecs::Entity entityA;
    ecs::Entity entityB;
    sf::Vector2f overlap;
    sf::Vector2f normal;
    float penetration;
    bool isTrigger;
};

struct CollisionEvent {
    enum Type { Enter, Stay, Exit };
    Type type;
    CollisionPair pair;
};

class CollisionSystem : public ecs::ISystem {
public:
    CollisionSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnRender(ecs::World* world, float alpha) override;

    void SetBroadPhaseEnabled(bool enabled);
    void SetDebugDraw(bool enabled);
    void SetCollisionCallback(std::function<void(const CollisionEvent&)> callback);

private:
    using CollisionKey = uint64_t;

    struct CollisionCacheEntry {
        bool wasColliding;
        float time;
    };

    void BroadPhase(ecs::World* world, std::vector<CollisionPair>& outPairs);
    void NarrowPhase(const std::vector<CollisionPair>& broad, std::vector<CollisionPair>& outPairs);
    bool TestAABBvsAABB(const components::CollisionComponent& a,
                        const components::CollisionComponent& b,
                        const sf::Vector2f& posA,
                        const sf::Vector2f& posB,
                        sf::Vector2f& outOverlap,
                        sf::Vector2f& outNormal,
                        float& outPenetration);
    bool TestCirclevsAABB(const components::CollisionComponent& circle,
                          const components::CollisionComponent& aabb,
                          const sf::Vector2f& posCircle,
                          const sf::Vector2f& posAABB);
    void ResolveCollision(components::CollisionComponent& a, components::CollisionComponent& b,
                          const CollisionPair& pair);
    void UpdateCollisionCache(const std::vector<CollisionPair>& currentPairs);
    void DispatchEvents(const std::vector<CollisionPair>& currentPairs);
    CollisionKey MakeKey(ecs::Entity a, ecs::Entity b) const;

    void DebugDrawPair(const CollisionPair& pair);

    bool m_broadPhaseEnabled;
    bool m_debugDraw;
    std::unordered_map<CollisionKey, CollisionCacheEntry> m_collisionCache;
    std::vector<std::function<void(const CollisionEvent&)>> m_callbacks;
    ecs::World* m_world;
};

} // namespace systems
} // namespace engine
