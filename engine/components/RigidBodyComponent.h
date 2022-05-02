#pragma once

#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"

namespace engine {
namespace components {

enum class BodyType {
    Static,
    Dynamic,
    Kinematic
};

class RigidBodyComponent : public ecs::Component<RigidBodyComponent> {
public:
    BodyType bodyType;

    float mass;
    float inverseMass;
    float inertia;
    float inverseInertia;

    float friction;
    float staticFriction;
    float rollingFriction;
    float restitution;

    sf::Vector2f centerOfMass;
    sf::Vector2f velocity;
    sf::Vector2f angularVelocity;

    float linearDamping;
    float angularDamping;
    float maxAngularVelocity;

    bool gravityEnabled;
    bool sleeping;
    bool continuousCollision;
    bool fixedRotation;

    float sleepThreshold;
    float sleepTimer;

    RigidBodyComponent();

    void SetBodyType(BodyType type);
    BodyType GetBodyType() const;

    void SetMass(float m);
    void SetInertia(float i);
    void SetFriction(float f);
    void SetRestitution(float r);

    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    void SetFixedRotation(bool fixed);

    void ApplyForce(const sf::Vector2f& force, const sf::Vector2f& point);
    void ApplyForceToCenter(const sf::Vector2f& force);
    void ApplyTorque(float torque);
    void ApplyImpulse(const sf::Vector2f& impulse, const sf::Vector2f& point);

    void SetVelocity(const sf::Vector2f& vel);
    void SetAngularVelocity(float omega);

    void WakeUp();
    void Sleep();
    bool IsSleeping() const;
    bool IsStatic() const;
    bool IsDynamic() const;

    void IntegrateForces(float deltaTime, const sf::Vector2f& gravity);
    void IntegrateVelocity(float deltaTime);

    float GetMass() const;
    float GetInverseMass() const;
    float GetInertia() const;
};

} // namespace components
} // namespace engine
