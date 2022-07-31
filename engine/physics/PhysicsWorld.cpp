#include "PhysicsWorld.h"
#include "CollisionDetection.h"
#include "ContactResolver.h"
#include <algorithm>

namespace engine { namespace physics {

RigidBody::RigidBody()
    : position(0, 0)
    , velocity(0, 0)
    , acceleration(0, 0)
    , mass(1.0f)
    , invMass(1.0f)
    , restitution(0.5f)
    , friction(0.3f)
    , width(32.0f)
    , height(32.0f)
    , radius(16.0f)
    , isStatic(false)
    , isCircle(false)
{
}

void RigidBody::applyForce(const engine::math::Vector2f& force) {
    if (isStatic) return;
    acceleration += force * invMass;
}

void RigidBody::applyImpulse(const engine::math::Vector2f& impulse) {
    if (isStatic) return;
    velocity += impulse * invMass;
}

engine::math::Rectf RigidBody::getAABB() const {
    if (isCircle) {
        return engine::math::Rectf(
            position.x - radius,
            position.y - radius,
            radius * 2.0f,
            radius * 2.0f
        );
    }
    return engine::math::Rectf(
        position.x - width * 0.5f,
        position.y - height * 0.5f,
        width,
        height
    );
}

PhysicsWorld::PhysicsWorld(const engine::math::Vector2f& gravity)
    : mGravity(gravity)
{
}

void PhysicsWorld::setGravity(const engine::math::Vector2f& gravity) {
    mGravity = gravity;
}

const engine::math::Vector2f& PhysicsWorld::getGravity() const {
    return mGravity;
}

RigidBody* PhysicsWorld::addBody(const RigidBody& body) {
    auto ptr = std::make_unique<RigidBody>(body);
    RigidBody* raw = ptr.get();
    mBodies.push_back(std::move(ptr));
    return raw;
}

void PhysicsWorld::removeBody(RigidBody* body) {
    auto it = std::remove_if(mBodies.begin(), mBodies.end(),
        [body](const std::unique_ptr<RigidBody>& b) { return b.get() == body; });
    mBodies.erase(it, mBodies.end());
}

void PhysicsWorld::clear() {
    mBodies.clear();
    mContacts.clear();
}

void PhysicsWorld::step(float dt, int iterations) {
    if (dt > 0.0167f) dt = 0.0167f;

    integrateForces(dt);
    detectCollisions();
    resolveContacts(iterations);
    integrateVelocities(dt);
}

void PhysicsWorld::integrateForces(float dt) {
    for (auto& body : mBodies) {
        if (body->isStatic) continue;
        body->acceleration += mGravity;
        body->velocity += body->acceleration * dt;
        body->acceleration = engine::math::Vector2f(0, 0);
    }
}

void PhysicsWorld::detectCollisions() {
    mContacts.clear();
    CollisionDetection detector;

    for (size_t i = 0; i < mBodies.size(); ++i) {
        for (size_t j = i + 1; j < mBodies.size(); ++j) {
            auto* a = mBodies[i].get();
            auto* b = mBodies[j].get();
            if (a->isStatic && b->isStatic) continue;

            Contact contact{};
            bool hit = false;

            if (a->isCircle && b->isCircle) {
                hit = detector.circleVsCircle(*a, *b, contact);
            } else if (!a->isCircle && !b->isCircle) {
                hit = detector.aabbVsAABB(*a, *b, contact);
            } else if (a->isCircle && !b->isCircle) {
                hit = detector.circleVsAABB(*a, *b, contact);
            } else {
                hit = detector.circleVsAABB(*b, *a, contact);
            }

            if (hit) {
                contact.a = a;
                contact.b = b;
                mContacts.push_back(contact);
            }
        }
    }
}

void PhysicsWorld::resolveContacts(int iterations) {
    ContactResolver resolver;
    for (int i = 0; i < iterations; ++i) {
        for (auto& contact : mContacts) {
            resolver.resolve(contact);
        }
    }
}

void PhysicsWorld::integrateVelocities(float dt) {
    for (auto& body : mBodies) {
        if (body->isStatic) continue;
        body->position += body->velocity * dt;
    }
}

RaycastHit PhysicsWorld::raycast(const engine::math::Vector2f& origin,
                                  const engine::math::Vector2f& direction,
                                  float maxDist) const
{
    RaycastHit closest;
    closest.hit = false;
    closest.distance = maxDist;

    for (const auto& body : mBodies) {
        auto aabb = body->getAABB();
        engine::math::Vector2f dir = direction.normalized();
        float tmin = (aabb.left() - origin.x) / dir.x;
        float tmax = (aabb.right() - origin.x) / dir.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (aabb.top() - origin.y) / dir.y;
        float tymax = (aabb.bottom() - origin.y) / dir.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        float t0 = std::max(tmin, tymin);
        float t1 = std::min(tmax, tymax);

        if (t0 > t1 || t1 < 0) continue;
        if (t0 < 0) t0 = t1;
        if (t0 >= closest.distance) continue;

        closest.hit = true;
        closest.body = body.get();
        closest.distance = t0;
        closest.point = origin + dir * t0;
        closest.normal = engine::math::Vector2f(0, 0);
    }
    return closest;
}

std::vector<RigidBody*> PhysicsWorld::queryAABB(const engine::math::Rectf& region) const {
    std::vector<RigidBody*> result;
    for (const auto& body : mBodies) {
        if (body->getAABB().intersects(region)) {
            result.push_back(body.get());
        }
    }
    return result;
}

}} // namespace engine::physics
