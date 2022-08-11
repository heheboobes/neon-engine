#include "ContactResolver.h"
#include <algorithm>
#include <cmath>

namespace engine { namespace physics {

void ContactResolver::resolve(Contact& contact) {
    resolvePenetration(contact);
    applyImpulse(contact);
    applyFriction(contact);
}

void ContactResolver::resolvePenetration(Contact& contact) {
    auto* a = contact.a;
    auto* b = contact.b;

    if (contact.penetration <= 0.0f) return;

    float totalInvMass = a->invMass + b->invMass;
    if (totalInvMass <= 0.0f) return;

    engine::math::Vector2f correction = contact.normal * (contact.penetration / totalInvMass);

    if (!a->isStatic) a->position -= correction * a->invMass;
    if (!b->isStatic) b->position += correction * b->invMass;
}

void ContactResolver::applyImpulse(Contact& contact) {
    auto* a = contact.a;
    auto* b = contact.b;

    engine::math::Vector2f relVel = b->velocity - a->velocity;
    float velAlongNormal = relVel.dot(contact.normal);

    if (velAlongNormal > 0.0f) return;

    float e = computeRestitution(*a, *b);
    float totalInvMass = a->invMass + b->invMass;
    if (totalInvMass <= 0.0f) return;

    float j = -(1.0f + e) * velAlongNormal / totalInvMass;

    engine::math::Vector2f impulse = contact.normal * j;

    a->applyImpulse(-impulse);
    b->applyImpulse(impulse);
}

void ContactResolver::applyFriction(Contact& contact) {
    auto* a = contact.a;
    auto* b = contact.b;

    engine::math::Vector2f relVel = b->velocity - a->velocity;

    engine::math::Vector2f tangent = relVel - contact.normal * relVel.dot(contact.normal);
    float tangentLen = tangent.length();
    if (tangentLen < 1e-6f) return;

    tangent = tangent / tangentLen;
    float mu = computeFriction(*a, *b);
    float totalInvMass = a->invMass + b->invMass;
    if (totalInvMass <= 0.0f) return;

    float jt = -relVel.dot(tangent) / totalInvMass;

    if (std::abs(jt) > mu * std::abs(relVel.dot(contact.normal)) / totalInvMass) {
        jt = -relVel.dot(tangent) / totalInvMass;
    }

    engine::math::Vector2f frictionImpulse = tangent * jt;

    a->applyImpulse(-frictionImpulse);
    b->applyImpulse(frictionImpulse);
}

float ContactResolver::computeRestitution(const RigidBody& a, const RigidBody& b) const {
    return std::min(a.restitution, b.restitution);
}

float ContactResolver::computeFriction(const RigidBody& a, const RigidBody& b) const {
    return std::sqrt(a.friction * b.friction);
}

}} // namespace engine::physics
