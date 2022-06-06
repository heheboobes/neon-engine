#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/TransformComponent.h"
#include <vector>
#include <unordered_set>

namespace engine {
namespace systems {

class TransformSystem : public ecs::ISystem {
public:
    TransformSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnEntityCreated(ecs::World* world, ecs::Entity entity) override;
    void OnEntityDestroyed(ecs::World* world, ecs::Entity entity) override;

    void MarkDirty(ecs::Entity entity);
    void MarkSubtreeDirty(ecs::Entity entity);
    void RebuildAll(ecs::World* world);

    sf::Vector3f GetWorldPosition(ecs::World* world, ecs::Entity entity) const;
    void SetWorldPosition(ecs::World* world, ecs::Entity entity, const sf::Vector3f& worldPos);

    void Reparent(ecs::World* world, ecs::Entity entity, ecs::Entity newParent);

private:
    struct TransformNode {
        ecs::Entity entity;
        components::TransformComponent* component;
        bool dirty;
    };

    void CollectRoots(ecs::World* world);
    void UpdateTransform(components::TransformComponent& transform);
    void PropagateDirty(components::TransformComponent* component);
    void SortByDepth(ecs::World* world);

    std::vector<TransformNode> m_roots;
    std::unordered_set<ecs::Entity> m_dirtyEntities;
    bool m_needsRebuild;
};

} // namespace systems
} // namespace engine
