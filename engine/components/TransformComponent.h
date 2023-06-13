#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include "../../engine/ecs/Component.h"
#include <vector>
#include <cstdint>

namespace engine {
namespace components {

class TransformComponent : public ecs::Component<TransformComponent> {
public:
    sf::Vector3f position;
    sf::Vector3f rotation;
    sf::Vector3f scale;

    TransformComponent* parent;
    std::vector<TransformComponent*> children;

    bool dirty;

    TransformComponent();
    explicit TransformComponent(const sf::Vector3f& pos);
    TransformComponent(const sf::Vector3f& pos, const sf::Vector3f& rot, const sf::Vector3f& scl);

    sf::Vector3f GetForward() const;
    sf::Vector3f GetUp() const;
    sf::Vector3f GetRight() const;

    void Translate(const sf::Vector3f& delta);
    void Rotate(const sf::Vector3f& delta);
    void SetScale(const sf::Vector3f& scl);

    void AddChild(TransformComponent* child);
    void RemoveChild(TransformComponent* child);
    bool IsChildOf(const TransformComponent* potentialParent) const;

    sf::Vector3f GetWorldPosition() const;
    void SetWorldPosition(const sf::Vector3f& worldPos);

    void MarkDirty();
};

} // namespace components
} // namespace engine
// v2: Added world matrix cache
// DOCS: Component docs
