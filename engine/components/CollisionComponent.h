#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"
#include <vector>
#include <cstdint>

namespace engine {
namespace components {

enum class CollisionShapeType {
    AABB,
    Circle,
    ConvexHull
};

struct CollisionLayer {
    static constexpr uint16_t Default = 1 << 0;
    static constexpr uint16_t Player = 1 << 1;
    static constexpr uint16_t Enemy = 1 << 2;
    static constexpr uint16_t Projectile = 1 << 3;
    static constexpr uint16_t Terrain = 1 << 4;
    static constexpr uint16_t Interactive = 1 << 5;
    static constexpr uint16_t Trigger = 1 << 6;
    static constexpr uint16_t All = 0xFFFF;
};

class CollisionComponent : public ecs::Component<CollisionComponent> {
public:
    CollisionShapeType shapeType;
    sf::FloatRect aabb;
    std::vector<sf::Vector2f> convexHull;
    float circleRadius;

    uint16_t layer;
    uint16_t mask;
    bool isTrigger;
    bool enabled;

    sf::Vector2f offset;
    float friction;
    float bounciness;

    CollisionComponent();

    void SetAABB(const sf::FloatRect& rect);
    void SetCircle(float radius, const sf::Vector2f& center);
    void SetConvexHull(const std::vector<sf::Vector2f>& hull);

    void SetLayer(uint16_t l);
    void SetMask(uint16_t m);
    void AddToLayer(uint16_t l);
    void RemoveFromLayer(uint16_t l);

    bool CanCollideWith(uint16_t otherLayer) const;
    bool IsInLayer(uint16_t l) const;

    void SetTrigger(bool trigger);
    bool IsTrigger() const;

    sf::FloatRect GetTransformedAABB(const sf::Vector2f& position) const;
    void SetOffset(const sf::Vector2f& off);
};

} // namespace components
} // namespace engine
