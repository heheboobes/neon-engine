#pragma once

#include "PhysicsWorld.h"

namespace engine { namespace physics {

class ContactResolver {
public:
    void resolve(Contact& contact);

private:
    void resolvePenetration(Contact& contact);
    void applyImpulse(Contact& contact);
    void applyFriction(Contact& contact);
    float computeRestitution(const RigidBody& a, const RigidBody& b) const;
    float computeFriction(const RigidBody& a, const RigidBody& b) const;
};

}} // namespace engine::physics
