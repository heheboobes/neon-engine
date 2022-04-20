#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"

namespace engine {
namespace components {

enum class CameraProjection {
    Orthographic,
    Perspective
};

struct CameraFollowTarget {
    int entityId;
    sf::Vector2f offset;
    bool lockX;
    bool lockY;
    bool smoothFollow;
    float followSpeed;
};

class CameraComponent : public ecs::Component<CameraComponent> {
public:
    sf::View view;
    sf::FloatRect viewport;

    CameraProjection projection;
    float zoom;
    float nearPlane;
    float farPlane;
    float fieldOfView;

    CameraFollowTarget followTarget;
    bool active;
    bool constrainToWorld;
    sf::FloatRect worldBounds;

    CameraComponent();

    void SetViewport(const sf::FloatRect& rect);
    void SetZoom(float z);
    void ZoomIn(float factor);
    void ZoomOut(float factor);

    void SetCenter(const sf::Vector2f& center);
    void SetSize(const sf::Vector2f& size);
    void SetRotation(float angle);

    sf::Vector2f GetCenter() const;
    sf::Vector2f GetSize() const;
    float GetRotation() const;
    float GetZoom() const;

    void FollowEntity(int entityId, const sf::Vector2f& offset = sf::Vector2f(0.f, 0.f));
    void StopFollowing();
    bool IsFollowing() const;

    void SetWorldBounds(const sf::FloatRect& bounds);
    void ConstrainToWorld(bool constrain);

    void Update(float deltaTime, const sf::Vector2f& targetPosition);

    sf::FloatRect GetViewBounds() const;
    sf::Vector2f ScreenToWorld(const sf::Vector2f& screenPos) const;
    sf::Vector2f WorldToScreen(const sf::Vector2f& worldPos) const;
};

} // namespace components
} // namespace engine
