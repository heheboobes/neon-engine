#pragma once

#include "PhysicsWorld.h"

namespace engine { namespace physics {

class CollisionDetection {
public:
    bool aabbVsAABB(const RigidBody& a, const RigidBody& b, Contact& contact) const;
    bool circleVsCircle(const RigidBody& a, const RigidBody& b, Contact& contact) const;
    bool circleVsAABB(const RigidBody& circle, const RigidBody& aabb, Contact& contact) const;
    bool pointVsAABB(const engine::math::Vector2f& point, const RigidBody& aabb) const;
    bool pointVsCircle(const engine::math::Vector2f& point, const RigidBody& circle) const;

private:
    engine::math::Vector2f closestPointOnAABB(const engine::math::Vector2f& p, const RigidBody& aabb) const;
};

}} // namespace engine::physics
