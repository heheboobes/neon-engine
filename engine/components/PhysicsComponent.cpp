#include "PhysicsComponent.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace components {

PhysicsComponent::PhysicsComponent()
    : velocity(0.f, 0.f)
    , acceleration(0.f, 0.f)
    , accumulatedForce(0.f, 0.f)
    , mass(1.0f)
    , drag(0.01f)
    , gravityScale(1.0f)
    , maxSpeed(1000.0f)
    , restitution(0.5f)
    , useGravity(true)
    , kinematic(false)
    , frozen(false)
{
}

void PhysicsComponent::ApplyForce(const sf::Vector2f& force)
{
    if (frozen || kinematic) return;
    accumulatedForce += force;
}

void PhysicsComponent::ApplyImpulse(const sf::Vector2f& impulse)
{
    if (frozen || kinematic) return;
    float invMass = GetInverseMass();
    velocity += impulse * invMass;
    ClampSpeed();
}

void PhysicsComponent::ApplyDrag(float deltaTime)
{
    if (frozen || drag <= 0.0f) return;
    float dragFactor = std::exp(-drag * deltaTime);
    velocity.x *= dragFactor;
    velocity.y *= dragFactor;
}

void PhysicsComponent::SetVelocity(const sf::Vector2f& vel)
{
    velocity = vel;
    ClampSpeed();
}

void PhysicsComponent::SetVelocityX(float vx)
{
    velocity.x = vx;
    ClampSpeed();
}

void PhysicsComponent::SetVelocityY(float vy)
{
    velocity.y = vy;
    ClampSpeed();
}

void PhysicsComponent::SetMass(float m)
{
    mass = std::max(0.0001f, m);
}

float PhysicsComponent::GetMass() const
{
    return mass;
}

float PhysicsComponent::GetInverseMass() const
{
    return (mass > 0.0001f) ? (1.0f / mass) : 0.0f;
}

void PhysicsComponent::SetGravityScale(float scale)
{
    gravityScale = scale;
}

void PhysicsComponent::SetDrag(float d)
{
    drag = std::max(0.0f, d);
}

void PhysicsComponent::ResetForces()
{
    accumulatedForce = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);
}

void PhysicsComponent::Freeze()
{
    frozen = true;
    velocity = sf::Vector2f(0.f, 0.f);
    acceleration = sf::Vector2f(0.f, 0.f);
}

void PhysicsComponent::Unfreeze()
{
    frozen = false;
}

void PhysicsComponent::Integrate(float deltaTime, const sf::Vector2f& gravity)
{
    if (frozen || kinematic) return;

    if (useGravity) {
        accumulatedForce += gravity * gravityScale * mass;
    }

    acceleration = accumulatedForce * GetInverseMass();
    velocity += acceleration * deltaTime;
    ApplyDrag(deltaTime);
    ClampSpeed();
    ResetForces();
}

sf::Vector2f PhysicsComponent::GetMomentum() const
{
    return velocity * mass;
}

float PhysicsComponent::GetKineticEnergy() const
{
    return 0.5f * mass * (velocity.x * velocity.x + velocity.y * velocity.y);
}

void PhysicsComponent::ClampSpeed()
{
    float speedSq = velocity.x * velocity.x + velocity.y * velocity.y;
    if (speedSq > maxSpeed * maxSpeed) {
        float speed = std::sqrt(speedSq);
        velocity.x = (velocity.x / speed) * maxSpeed;
        velocity.y = (velocity.y / speed) * maxSpeed;
    }
}

} // namespace components
} // namespace engine
