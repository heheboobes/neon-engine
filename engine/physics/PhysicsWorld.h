#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "../math/Vector2.h"
#include "../math/Rect.h"

namespace engine { namespace physics {

struct RigidBody {
    engine::math::Vector2f position;
    engine::math::Vector2f velocity;
    engine::math::Vector2f acceleration;
    float mass;
    float invMass;
    float restitution;
    float friction;
    float width;
    float height;
    float radius;
    bool isStatic;
    bool isCircle;

    RigidBody();
    void applyForce(const engine::math::Vector2f& force);
    void applyImpulse(const engine::math::Vector2f& impulse);
    engine::math::Rectf getAABB() const;
};

struct Contact {
    RigidBody* a;
    RigidBody* b;
    engine::math::Vector2f normal;
    float penetration;
    engine::math::Vector2f contactPoint;
};

struct RaycastHit {
    RigidBody* body;
    engine::math::Vector2f point;
    engine::math::Vector2f normal;
    float distance;
    bool hit;
};

class PhysicsWorld {
public:
    explicit PhysicsWorld(const engine::math::Vector2f& gravity = engine::math::Vector2f(0.0f, 980.0f));

    void setGravity(const engine::math::Vector2f& gravity);
    const engine::math::Vector2f& getGravity() const;

    RigidBody* addBody(const RigidBody& body);
    void removeBody(RigidBody* body);
    void clear();

    void step(float dt, int iterations = 8);

    RaycastHit raycast(const engine::math::Vector2f& origin, const engine::math::Vector2f& direction, float maxDist) const;
    std::vector<RigidBody*> queryAABB(const engine::math::Rectf& region) const;

private:
    engine::math::Vector2f mGravity;
    std::vector<std::unique_ptr<RigidBody>> mBodies;
    std::vector<Contact> mContacts;

    void integrateForces(float dt);
    void detectCollisions();
    void resolveContacts(int iterations);
    void integrateVelocities(float dt);
};

}} // namespace engine::physics
// v2: Added contact pair caching
