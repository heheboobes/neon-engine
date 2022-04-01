#include "TransformComponent.h"
#include <cmath>
#include <algorithm>

namespace engine {
namespace components {

TransformComponent::TransformComponent()
    : position(0.f, 0.f, 0.f)
    , rotation(0.f, 0.f, 0.f)
    , scale(1.f, 1.f, 1.f)
    , parent(nullptr)
    , dirty(true)
{
}

TransformComponent::TransformComponent(const sf::Vector3f& pos)
    : position(pos)
    , rotation(0.f, 0.f, 0.f)
    , scale(1.f, 1.f, 1.f)
    , parent(nullptr)
    , dirty(true)
{
}

TransformComponent::TransformComponent(const sf::Vector3f& pos, const sf::Vector3f& rot, const sf::Vector3f& scl)
    : position(pos)
    , rotation(rot)
    , scale(scl)
    , parent(nullptr)
    , dirty(true)
{
}

sf::Vector3f TransformComponent::GetForward() const
{
    float cy = std::cos(rotation.y * 3.14159265f / 180.f);
    float sy = std::sin(rotation.y * 3.14159265f / 180.f);
    float cp = std::cos(rotation.x * 3.14159265f / 180.f);
    float sp = std::sin(rotation.x * 3.14159265f / 180.f);
    return sf::Vector3f(-sy * cp, sp, cy * cp);
}

sf::Vector3f TransformComponent::GetUp() const
{
    sf::Vector3f fwd = GetForward();
    sf::Vector3f right = GetRight();
    return sf::Vector3f(
        fwd.y * right.z - fwd.z * right.y,
        fwd.z * right.x - fwd.x * right.z,
        fwd.x * right.y - fwd.y * right.x
    );
}

sf::Vector3f TransformComponent::GetRight() const
{
    float cy = std::cos(rotation.y * 3.14159265f / 180.f);
    float sy = std::sin(rotation.y * 3.14159265f / 180.f);
    return sf::Vector3f(cy, 0.f, -sy);
}

void TransformComponent::Translate(const sf::Vector3f& delta)
{
    position.x += delta.x;
    position.y += delta.y;
    position.z += delta.z;
    MarkDirty();
}

void TransformComponent::Rotate(const sf::Vector3f& delta)
{
    rotation.x += delta.x;
    rotation.y += delta.y;
    rotation.z += delta.z;
    MarkDirty();
}

void TransformComponent::SetScale(const sf::Vector3f& scl)
{
    scale = scl;
    MarkDirty();
}

void TransformComponent::AddChild(TransformComponent* child)
{
    if (child && child != this && !IsChildOf(child)) {
        if (child->parent) {
            child->parent->RemoveChild(child);
        }
        child->parent = this;
        children.push_back(child);
        child->MarkDirty();
    }
}

void TransformComponent::RemoveChild(TransformComponent* child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
        child->MarkDirty();
    }
}

bool TransformComponent::IsChildOf(const TransformComponent* potentialParent) const
{
    const TransformComponent* cur = parent;
    while (cur) {
        if (cur == potentialParent) return true;
        cur = cur->parent;
    }
    return false;
}

sf::Vector3f TransformComponent::GetWorldPosition() const
{
    if (!parent) return position;
    sf::Vector3f parentWorld = parent->GetWorldPosition();
    return sf::Vector3f(
        parentWorld.x + position.x * parent->scale.x,
        parentWorld.y + position.y * parent->scale.y,
        parentWorld.z + position.z * parent->scale.z
    );
}

void TransformComponent::SetWorldPosition(const sf::Vector3f& worldPos)
{
    if (!parent) {
        position = worldPos;
    } else {
        sf::Vector3f parentWorld = parent->GetWorldPosition();
        position = sf::Vector3f(
            (worldPos.x - parentWorld.x) / parent->scale.x,
            (worldPos.y - parentWorld.y) / parent->scale.y,
            (worldPos.z - parentWorld.z) / parent->scale.z
        );
    }
    MarkDirty();
}

void TransformComponent::MarkDirty()
{
    dirty = true;
    for (auto* child : children) {
        child->MarkDirty();
    }
}

} // namespace components
} // namespace engine
