#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/PhysicsComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/System/Vector2.hpp>

namespace engine {
namespace systems {

class PhysicsSystem : public ecs::ISystem {
public:
    PhysicsSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;

    void SetGravity(const sf::Vector2f& gravity);
    sf::Vector2f GetGravity() const;
    void SetVelocityIterations(int32_t iterations);
    void SetEnabled(bool enabled);

private:
    void IntegrateForces(components::PhysicsComponent& physics, float dt);
    void IntegrateVelocity(components::PhysicsComponent& physics, components::TransformComponent& transform, float dt);
    void ApplyDamping(components::PhysicsComponent& physics, float dt);
    void ClampVelocity(components::PhysicsComponent& physics);

    sf::Vector2f m_gravity;
    int32_t m_velocityIterations;
    bool m_enabled;
};

} // namespace systems
} // namespace engine
