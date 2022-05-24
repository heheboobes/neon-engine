#include "PhysicsSystem.h"
#include "../ecs/World.h"
#include <cmath>

namespace engine {
namespace systems {

PhysicsSystem::PhysicsSystem()
    : m_gravity(0.0f, 9.81f * 50.0f)
    , m_velocityIterations(1)
    , m_enabled(true)
{
    SetName("PhysicsSystem");
    SetPriority(50);
    Require<components::PhysicsComponent, components::TransformComponent>();
}

void PhysicsSystem::SetGravity(const sf::Vector2f& gravity)
{
    m_gravity = gravity;
}

sf::Vector2f PhysicsSystem::GetGravity() const
{
    return m_gravity;
}

void PhysicsSystem::SetVelocityIterations(int32_t iterations)
{
    m_velocityIterations = std::max(1, iterations);
}

void PhysicsSystem::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

void PhysicsSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    if (!m_enabled || deltaTime <= 0.0f)
        return;

    float subDt = deltaTime / static_cast<float>(m_velocityIterations);

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* physics = static_cast<components::PhysicsComponent*>(
                chunk.GetComponentData(components::PhysicsComponent::GetStaticTypeID()));
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!physics || !transforms)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& p = physics[i];
                if (p.frozen || p.kinematic)
                    continue;

                for (int iter = 0; iter < m_velocityIterations; ++iter)
                {
                    IntegrateForces(p, subDt);
                    ApplyDamping(p, subDt);
                    IntegrateVelocity(p, transforms[i], subDt);
                    ClampVelocity(p);
                }
            }
        }
    }
}

void PhysicsSystem::IntegrateForces(components::PhysicsComponent& physics, float dt)
{
    if (physics.useGravity && !physics.kinematic)
    {
        sf::Vector2f gravityForce = m_gravity * physics.gravityScale * physics.mass;
        physics.accumulatedForce += gravityForce;
    }

    sf::Vector2f acceleration = physics.accumulatedForce * physics.GetInverseMass();
    physics.acceleration = acceleration;
    physics.velocity += acceleration * dt;
    physics.ResetForces();
}

void PhysicsSystem::IntegrateVelocity(components::PhysicsComponent& physics,
                                       components::TransformComponent& transform, float dt)
{
    transform.position.x += physics.velocity.x * dt;
    transform.position.y += physics.velocity.y * dt;
    transform.MarkDirty();
}

void PhysicsSystem::ApplyDamping(components::PhysicsComponent& physics, float dt)
{
    if (physics.drag > 0.0f)
    {
        float dampingFactor = 1.0f - physics.drag * dt;
        if (dampingFactor < 0.0f)
            dampingFactor = 0.0f;
        physics.velocity *= dampingFactor;
    }
}

void PhysicsSystem::ClampVelocity(components::PhysicsComponent& physics)
{
    if (physics.maxSpeed > 0.0f)
    {
        float speed = std::sqrt(physics.velocity.x * physics.velocity.x +
                                physics.velocity.y * physics.velocity.y);
        if (speed > physics.maxSpeed)
        {
            float scale = physics.maxSpeed / speed;
            physics.velocity *= scale;
        }
    }
}

} // namespace systems
} // namespace engine
// v2: Added force accumulation
