#include "CameraComponent.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace components {

CameraComponent::CameraComponent()
    : viewport(0.f, 0.f, 1.f, 1.f)
    , projection(CameraProjection::Orthographic)
    , zoom(1.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , fieldOfView(60.0f)
    , active(true)
    , constrainToWorld(false)
    , worldBounds(0.f, 0.f, 10000.f, 10000.f)
{
    followTarget.entityId = -1;
    followTarget.offset = sf::Vector2f(0.f, 0.f);
    followTarget.lockX = false;
    followTarget.lockY = false;
    followTarget.smoothFollow = true;
    followTarget.followSpeed = 5.0f;
    view.setViewport(viewport);
}

void CameraComponent::SetViewport(const sf::FloatRect& rect)
{
    viewport = rect;
    view.setViewport(viewport);
}

void CameraComponent::SetZoom(float z)
{
    zoom = std::max(0.01f, z);
    sf::Vector2f size = view.getSize();
    view.setSize(size.x * zoom, size.y * zoom);
}

void CameraComponent::ZoomIn(float factor)
{
    SetZoom(zoom / std::max(0.01f, factor));
}

void CameraComponent::ZoomOut(float factor)
{
    SetZoom(zoom * std::max(1.0f, factor));
}

void CameraComponent::SetCenter(const sf::Vector2f& center)
{
    view.setCenter(center);
}

void CameraComponent::SetSize(const sf::Vector2f& size)
{
    view.setSize(size.x * zoom, size.y * zoom);
}

void CameraComponent::SetRotation(float angle)
{
    view.setRotation(angle);
}

sf::Vector2f CameraComponent::GetCenter() const
{
    return view.getCenter();
}

sf::Vector2f CameraComponent::GetSize() const
{
    return view.getSize();
}

float CameraComponent::GetRotation() const
{
    return view.getRotation();
}

float CameraComponent::GetZoom() const
{
    return zoom;
}

void CameraComponent::FollowEntity(int entityId, const sf::Vector2f& offset)
{
    followTarget.entityId = entityId;
    followTarget.offset = offset;
    followTarget.lockX = false;
    followTarget.lockY = false;
}

void CameraComponent::StopFollowing()
{
    followTarget.entityId = -1;
}

bool CameraComponent::IsFollowing() const
{
    return followTarget.entityId >= 0;
}

void CameraComponent::SetWorldBounds(const sf::FloatRect& bounds)
{
    worldBounds = bounds;
}

void CameraComponent::ConstrainToWorld(bool constrain)
{
    constrainToWorld = constrain;
}

void CameraComponent::Update(float deltaTime, const sf::Vector2f& targetPosition)
{
    if (!active) return;

    if (IsFollowing()) {
        sf::Vector2f target = targetPosition + followTarget.offset;

        if (followTarget.lockX) target.x = view.getCenter().x;
        if (followTarget.lockY) target.y = view.getCenter().y;

        if (followTarget.smoothFollow) {
            sf::Vector2f current = view.getCenter();
            float t = std::min(1.0f, followTarget.followSpeed * deltaTime);
            target.x = std::lerp(current.x, target.x, t);
            target.y = std::lerp(current.y, target.y, t);
        }

        view.setCenter(target);
    }

    if (constrainToWorld) {
        sf::Vector2f center = view.getCenter();
        sf::Vector2f halfSize = view.getSize() / 2.f;

        center.x = std::max(worldBounds.left + halfSize.x, std::min(center.x, worldBounds.left + worldBounds.width - halfSize.x));
        center.y = std::max(worldBounds.top + halfSize.y, std::min(center.y, worldBounds.top + worldBounds.height - halfSize.y));

        view.setCenter(center);
    }
}

sf::FloatRect CameraComponent::GetViewBounds() const
{
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();
    return sf::FloatRect(
        center.x - size.x / 2.f,
        center.y - size.y / 2.f,
        size.x,
        size.y
    );
}

sf::Vector2f CameraComponent::ScreenToWorld(const sf::Vector2f& screenPos) const
{
    sf::Vector2f normalized(
        screenPos.x / viewport.width,
        screenPos.y / viewport.height
    );
    sf::Vector2f halfSize = view.getSize() / 2.f;
    return sf::Vector2f(
        view.getCenter().x - halfSize.x + normalized.x * view.getSize().x,
        view.getCenter().y - halfSize.y + normalized.y * view.getSize().y
    );
}

sf::Vector2f CameraComponent::WorldToScreen(const sf::Vector2f& worldPos) const
{
    sf::Vector2f halfSize = view.getSize() / 2.f;
    sf::Vector2f relative(
        worldPos.x - (view.getCenter().x - halfSize.x),
        worldPos.y - (view.getCenter().y - halfSize.y)
    );
    return sf::Vector2f(
        (relative.x / view.getSize().x) * viewport.width,
        (relative.y / view.getSize().y) * viewport.height
    );
}

} // namespace components
} // namespace engine
