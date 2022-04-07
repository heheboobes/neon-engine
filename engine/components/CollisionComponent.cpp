#include "CollisionComponent.h"
#include <algorithm>

namespace engine {
namespace components {

CollisionComponent::CollisionComponent()
    : shapeType(CollisionShapeType::AABB)
    , aabb(0.f, 0.f, 32.f, 32.f)
    , circleRadius(16.f)
    , layer(CollisionLayer::Default)
    , mask(CollisionLayer::All)
    , isTrigger(false)
    , enabled(true)
    , offset(0.f, 0.f)
    , friction(0.3f)
    , bounciness(0.2f)
{
}

void CollisionComponent::SetAABB(const sf::FloatRect& rect)
{
    shapeType = CollisionShapeType::AABB;
    aabb = rect;
}

void CollisionComponent::SetCircle(float radius, const sf::Vector2f& center)
{
    shapeType = CollisionShapeType::Circle;
    circleRadius = radius;
    aabb = sf::FloatRect(
        center.x - radius, center.y - radius,
        radius * 2.f, radius * 2.f
    );
}

void CollisionComponent::SetConvexHull(const std::vector<sf::Vector2f>& hull)
{
    shapeType = CollisionShapeType::ConvexHull;
    convexHull = hull;

    if (!hull.empty()) {
        float minX = hull[0].x, minY = hull[0].y;
        float maxX = hull[0].x, maxY = hull[0].y;
        for (const auto& p : hull) {
            minX = std::min(minX, p.x);
            minY = std::min(minY, p.y);
            maxX = std::max(maxX, p.x);
            maxY = std::max(maxY, p.y);
        }
        aabb = sf::FloatRect(minX, minY, maxX - minX, maxY - minY);
    }
}

void CollisionComponent::SetLayer(uint16_t l)
{
    layer = l;
}

void CollisionComponent::SetMask(uint16_t m)
{
    mask = m;
}

void CollisionComponent::AddToLayer(uint16_t l)
{
    layer |= l;
}

void CollisionComponent::RemoveFromLayer(uint16_t l)
{
    layer &= ~l;
}

bool CollisionComponent::CanCollideWith(uint16_t otherLayer) const
{
    return enabled && (mask & otherLayer) != 0;
}

bool CollisionComponent::IsInLayer(uint16_t l) const
{
    return (layer & l) != 0;
}

void CollisionComponent::SetTrigger(bool trigger)
{
    isTrigger = trigger;
}

bool CollisionComponent::IsTrigger() const
{
    return isTrigger;
}

sf::FloatRect CollisionComponent::GetTransformedAABB(const sf::Vector2f& position) const
{
    return sf::FloatRect(
        aabb.left + position.x + offset.x,
        aabb.top + position.y + offset.y,
        aabb.width,
        aabb.height
    );
}

void CollisionComponent::SetOffset(const sf::Vector2f& off)
{
    offset = off;
}

} // namespace components
} // namespace engine
