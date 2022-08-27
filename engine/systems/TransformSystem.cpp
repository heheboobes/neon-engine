#include "TransformSystem.h"
#include "../ecs/World.h"
#include <queue>

namespace engine {
namespace systems {

TransformSystem::TransformSystem()
    : m_needsRebuild(true)
{
    SetName("TransformSystem");
    SetPriority(5);
    Require<components::TransformComponent>();
}

void TransformSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    (void)deltaTime;

    if (m_needsRebuild)
    {
        SortByDepth(world);
        m_needsRebuild = false;
    }

    if (m_dirtyEntities.empty())
        return;

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!transforms)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& transform = transforms[i];
                if (!transform.dirty)
                    continue;

                if (transform.parent && transform.parent->dirty)
                    continue;

                UpdateTransform(transform);
                transform.dirty = false;
            }
        }
    }

    m_dirtyEntities.clear();
}

void TransformSystem::OnEntityCreated(ecs::World* world, ecs::Entity entity)
{
    (void)world;
    (void)entity;
    m_needsRebuild = true;
}

void TransformSystem::OnEntityDestroyed(ecs::World* world, ecs::Entity entity)
{
    auto* transform = world->GetComponent<components::TransformComponent>(entity);
    if (transform)
    {
        if (transform->parent)
        {
            transform->parent->RemoveChild(transform);
        }
        for (auto* child : transform->children)
        {
            child->parent = nullptr;
        }
    }
    m_needsRebuild = true;
}

void TransformSystem::MarkDirty(ecs::Entity entity)
{
    m_dirtyEntities.insert(entity);
}

void TransformSystem::MarkSubtreeDirty(ecs::Entity entity)
{
    m_dirtyEntities.insert(entity);
}

void TransformSystem::RebuildAll(ecs::World* world)
{
    m_needsRebuild = true;
    OnUpdate(world, 0.0f);
}

sf::Vector3f TransformSystem::GetWorldPosition(ecs::World* world, ecs::Entity entity) const
{
    auto* transform = world->GetComponent<components::TransformComponent>(entity);
    if (!transform)
        return sf::Vector3f(0.0f, 0.0f, 0.0f);

    return transform->GetWorldPosition();
}

void TransformSystem::SetWorldPosition(ecs::World* world, ecs::Entity entity,
                                        const sf::Vector3f& worldPos)
{
    auto* transform = world->GetComponent<components::TransformComponent>(entity);
    if (!transform)
        return;

    transform->SetWorldPosition(worldPos);
}

void TransformSystem::Reparent(ecs::World* world, ecs::Entity entity, ecs::Entity newParent)
{
    auto* transform = world->GetComponent<components::TransformComponent>(entity);
    auto* parentTransform = world->GetComponent<components::TransformComponent>(newParent);

    if (!transform || !parentTransform)
        return;

    if (transform->parent)
    {
        transform->parent->RemoveChild(transform);
    }

    parentTransform->AddChild(transform);
    transform->MarkDirty();
    m_needsRebuild = true;
}

void TransformSystem::CollectRoots(ecs::World* world)
{
    m_roots.clear();

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!transforms)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                if (!transforms[i].parent)
                {
                    TransformNode node;
                    node.entity = chunk.GetEntity(i);
                    node.component = &transforms[i];
                    node.dirty = transforms[i].dirty;
                    m_roots.push_back(node);
                }
            }
        }
    }
}

void TransformSystem::UpdateTransform(components::TransformComponent& transform)
{
    for (auto* child : transform.children)
    {
        sf::Vector3f childWorldPos = child->GetWorldPosition();
        child->MarkDirty();
        child->dirty = false;

        if (child->children.empty())
            continue;

        std::queue<components::TransformComponent*> queue;
        queue.push(child);

        while (!queue.empty())
        {
            auto* current = queue.front();
            queue.pop();

            current->dirty = false;

            for (auto* grandchild : current->children)
            {
                grandchild->MarkDirty();
                queue.push(grandchild);
            }
        }
    }
}

void TransformSystem::PropagateDirty(components::TransformComponent* transform)
{
    transform->MarkDirty();
    for (auto* child : transform->children)
    {
        PropagateDirty(child);
    }
}

void TransformSystem::SortByDepth(ecs::World* world)
{
    CollectRoots(world);

    std::sort(m_roots.begin(), m_roots.end(),
        [](const TransformNode& a, const TransformNode& b) {
            return a.component->position.z < b.component->position.z;
        });
}

} // namespace systems
} // namespace engine

