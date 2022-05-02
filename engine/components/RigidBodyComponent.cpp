#include "RigidBodyComponent.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace components {

RigidBodyComponent::RigidBodyComponent()
    : bodyType(BodyType::Dynamic)
    , mass(1.0f)
    , inverseMass(1.0f)
    , inertia(1.0f)
    , inverseInertia(1.0f)
    , friction(0.3f)
    , staticFriction(0.5f)
    , rollingFriction(0.01f)
    , restitution(0.2f)
    , centerOfMass(0.f, 0.f)
    , velocity(0.f, 0.f)
    , angularVelocity(0.f, 0.f)
    , linearDamping(0.01f)
    , angularDamping(0.01f)
    , maxAngularVelocity(100.0f)
    , gravityEnabled(true)
    , sleeping(false)
    , continuousCollision(false)
    , fixedRotation(false)
    , sleepThreshold(0.01f)
    , sleepTimer(0.0f)
{
}

void RigidBodyComponent::SetBodyType(BodyType type)
{
    bodyType = type;
    if (type == BodyType::Static) {
        mass = 0.0f;
        inverseMass = 0.0f;
        inertia = 0.0f;
        inverseInertia = 0.0f;
        velocity = sf::Vector2f(0.f, 0.f);
        angularVelocity = sf::Vector2f(0.f, 0.f);
    } else if (type == BodyType::Kinematic) {
        mass = 1.0f;
        inverseMass = 1.0f;
        inertia = 1.0f;
        inverseInertia = 1.0f;
    }
}

BodyType RigidBodyComponent::GetBodyType() const
{
    return bodyType;
}

void RigidBodyComponent::SetMass(float m)
{
    mass = std::max(0.0001f, m);
    inverseMass = 1.0f / mass;
}

void RigidBodyComponent::SetInertia(float i)
{
    inertia = std::max(0.0001f, i);
    inverseInertia = 1.0f / inertia;
}

void RigidBodyComponent::SetFriction(float f)
{
    friction = std::clamp(f, 0.0f, 1.0f);
}

void RigidBodyComponent::SetRestitution(float r)
{
    restitution = std::clamp(r, 0.0f, 1.0f);
}

void RigidBodyComponent::SetLinearDamping(float damping)
{
    linearDamping = std::max(0.0f, damping);
}

void RigidBodyComponent::SetAngularDamping(float damping)
{
    angularDamping = std::max(0.0f, damping);
}

void RigidBodyComponent::SetFixedRotation(bool fixed)
{
    fixedRotation = fixed;
}

void RigidBodyComponent::ApplyForce(const sf::Vector2f& force, const sf::Vector2f& point)
{
    if (bodyType != BodyType::Dynamic || sleeping) return;
    velocity += force * inverseMass;
    sf::Vector2f r = point - centerOfMass;
    angularVelocity.x += (r.y * force.x - r.x * force.y) * inverseInertia;
}

void RigidBodyComponent::ApplyForceToCenter(const sf::Vector2f& force)
{
    if (bodyType != BodyType::Dynamic || sleeping) return;
    velocity += force * inverseMass;
}

void RigidBodyComponent::ApplyTorque(float torque)
{
    if (bodyType != BodyType::Dynamic || sleeping || fixedRotation) return;
    angularVelocity.x += torque * inverseInertia;
    ClampAngularVelocity();
}

void RigidBodyComponent::ApplyImpulse(const sf::Vector2f& impulse, const sf::Vector2f& point)
{
    if (bodyType != BodyType::Dynamic || sleeping) return;
    velocity += impulse * inverseMass;
    sf::Vector2f r = point - centerOfMass;
    angularVelocity.x += (r.y * impulse.x - r.x * impulse.y) * inverseInertia;
    ClampAngularVelocity();
}

void RigidBodyComponent::SetVelocity(const sf::Vector2f& vel)
{
    velocity = vel;
    WakeUp();
}

void RigidBodyComponent::SetAngularVelocity(float omega)
{
    angularVelocity.x = omega;
    WakeUp();
}

void RigidBodyComponent::WakeUp()
{
    sleeping = false;
    sleepTimer = 0.0f;
}

void RigidBodyComponent::Sleep()
{
    sleeping = true;
    velocity = sf::Vector2f(0.f, 0.f);
    angularVelocity = sf::Vector2f(0.f, 0.f);
}

bool RigidBodyComponent::IsSleeping() const
{
    return sleeping;
}

bool RigidBodyComponent::IsStatic() const
{
    return bodyType == BodyType::Static;
}

bool RigidBodyComponent::IsDynamic() const
{
    return bodyType == BodyType::Dynamic;
}

void RigidBodyComponent::IntegrateForces(float deltaTime, const sf::Vector2f& gravity)
{
    if (bodyType != BodyType::Dynamic || sleeping) return;

    if (gravityEnabled) {
        velocity += gravity * deltaTime;
    }

    velocity *= std::exp(-linearDamping * deltaTime);
    angularVelocity.x *= std::exp(-angularDamping * deltaTime);
    ClampAngularVelocity();

    float speedSq = velocity.x * velocity.x + velocity.y * velocity.y;
    if (speedSq < sleepThreshold * sleepThreshold) {
        sleepTimer += deltaTime;
        if (sleepTimer > 0.5f) Sleep();
    } else {
        sleepTimer = 0.0f;
    }
}

void RigidBodyComponent::IntegrateVelocity(float deltaTime)
{
    if (bodyType != BodyType::Dynamic || sleeping) return;
    // Position update handled externally by physics system
}

float RigidBodyComponent::GetMass() const
{
    return mass;
}

float RigidBodyComponent::GetInverseMass() const
{
    return inverseMass;
}

float RigidBodyComponent::GetInertia() const
{
    return inertia;
}

void RigidBodyComponent::ClampAngularVelocity()
{
    if (fixedRotation) {
        angularVelocity.x = 0.0f;
        return;
    }
    float absOmega = std::abs(angularVelocity.x);
    if (absOmega > maxAngularVelocity) {
        angularVelocity.x = (angularVelocity.x / absOmega) * maxAngularVelocity;
    }
}

} // namespace components
} // namespace engine
