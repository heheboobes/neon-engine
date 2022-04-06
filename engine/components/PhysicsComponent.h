#pragma once

#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"

namespace engine {
namespace components {

class PhysicsComponent : public ecs::Component<PhysicsComponent> {
public:
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::Vector2f accumulatedForce;

    float mass;
    float drag;
    float gravityScale;
    float maxSpeed;
    float restitution;

    bool useGravity;
    bool kinematic;
    bool frozen;

    PhysicsComponent();

    void ApplyForce(const sf::Vector2f& force);
    void ApplyImpulse(const sf::Vector2f& impulse);
    void ApplyDrag(float deltaTime);

    void SetVelocity(const sf::Vector2f& vel);
    void SetVelocityX(float vx);
    void SetVelocityY(float vy);

    void SetMass(float m);
    float GetMass() const;
    float GetInverseMass() const;

    void SetGravityScale(float scale);
    void SetDrag(float d);

    void ResetForces();
    void Freeze();
    void Unfreeze();

    void Integrate(float deltaTime, const sf::Vector2f& gravity);

    sf::Vector2f GetMomentum() const;
    float GetKineticEnergy() const;
};

} // namespace components
} // namespace engine
