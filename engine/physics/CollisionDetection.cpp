#include "CollisionDetection.h"
#include <algorithm>
#include <cmath>

namespace engine { namespace physics {

bool CollisionDetection::aabbVsAABB(const RigidBody& a, const RigidBody& b, Contact& contact) const {
    auto ax = a.getAABB();
    auto bx = b.getAABB();

    if (!ax.intersects(bx)) return false;

    float overlapLeft   = (ax.right()  - bx.left());
    float overlapRight  = (bx.right()  - ax.left());
    float overlapTop    = (ax.bottom() - bx.top());
    float overlapBottom = (bx.bottom() - ax.top());

    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});

    if (minOverlap == overlapLeft) {
        contact.normal = engine::math::Vector2f(-1, 0);
        contact.penetration = overlapLeft;
    } else if (minOverlap == overlapRight) {
        contact.normal = engine::math::Vector2f(1, 0);
        contact.penetration = overlapRight;
    } else if (minOverlap == overlapTop) {
        contact.normal = engine::math::Vector2f(0, -1);
        contact.penetration = overlapTop;
    } else {
        contact.normal = engine::math::Vector2f(0, 1);
        contact.penetration = overlapBottom;
    }

    contact.contactPoint = engine::math::Vector2f(
        std::max(ax.left(), bx.left()) + std::min(ax.right(), bx.right())
    ) * 0.5f;
    contact.contactPoint.y = std::max(ax.top(), bx.top()) + std::min(ax.bottom(), bx.bottom());
    contact.contactPoint *= 0.5f;

    return true;
}

bool CollisionDetection::circleVsCircle(const RigidBody& a, const RigidBody& b, Contact& contact) const {
    engine::math::Vector2f diff = b.position - a.position;
    float distSq = diff.lengthSq();
    float radSum = a.radius + b.radius;

    if (distSq >= radSum * radSum) return false;

    float dist = std::sqrt(distSq);
    if (dist < 1e-8f) {
        contact.normal = engine::math::Vector2f(0, -1);
        contact.penetration = radSum;
        contact.contactPoint = a.position;
        return true;
    }

    contact.normal = diff / dist;
    contact.penetration = radSum - dist;
    contact.contactPoint = a.position + contact.normal * (dist * 0.5f);
    return true;
}

bool CollisionDetection::circleVsAABB(const RigidBody& circle, const RigidBody& aabb, Contact& contact) const {
    engine::math::Vector2f closest = closestPointOnAABB(circle.position, aabb);
    engine::math::Vector2f diff = circle.position - closest;
    float distSq = diff.lengthSq();

    if (distSq >= circle.radius * circle.radius) return false;

    float dist = std::sqrt(distSq);
    if (dist < 1e-8f) {
        engine::math::Vector2f centerToAABB = circle.position - aabb.position;
        if (std::abs(centerToAABB.x) > std::abs(centerToAABB.y)) {
            contact.normal = engine::math::Vector2f(centerToAABB.x > 0 ? 1 : -1, 0);
        } else {
            contact.normal = engine::math::Vector2f(0, centerToAABB.y > 0 ? 1 : -1);
        }
        contact.penetration = circle.radius;
        contact.contactPoint = closest;
        return true;
    }

    contact.normal = diff / dist;
    contact.penetration = circle.radius - dist;
    contact.contactPoint = closest;
    return true;
}

bool CollisionDetection::pointVsAABB(const engine::math::Vector2f& point, const RigidBody& aabb) const {
    return aabb.getAABB().contains(point);
}

bool CollisionDetection::pointVsCircle(const engine::math::Vector2f& point, const RigidBody& circle) const {
    return (point - circle.position).lengthSq() <= circle.radius * circle.radius;
}

engine::math::Vector2f CollisionDetection::closestPointOnAABB(const engine::math::Vector2f& p, const RigidBody& aabb) const {
    auto r = aabb.getAABB();
    return engine::math::Vector2f(
        std::max(r.left(), std::min(p.x, r.right())),
        std::max(r.top(), std::min(p.y, r.bottom()))
    );
}

}} // namespace engine::physics
