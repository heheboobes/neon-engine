#include "CameraSystem.h"
#include "../ecs/World.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace systems {

CameraSystem::CameraSystem()
    : m_renderTarget(nullptr)
    , m_defaultViewSize(1280.0f, 720.0f)
    , m_hasActiveCamera(false)
{
    SetName("CameraSystem");
    SetPriority(90);
    Require<components::CameraComponent>();

    m_shake.intensity = 0.0f;
    m_shake.duration = 0.0f;
    m_shake.elapsed = 0.0f;
    m_shake.active = false;

    m_fade.duration = 0.0f;
    m_fade.elapsed = 0.0f;
    m_fade.fadingIn = true;
    m_fade.active = false;
    m_fade.color = sf::Color::Black;
}

void CameraSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* cameras = static_cast<components::CameraComponent*>(
                chunk.GetComponentData(components::CameraComponent::GetStaticTypeID()));

            if (!cameras)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& camera = cameras[i];
                if (!camera.active)
                    continue;

                if (camera.IsFollowing())
                {
                    auto* targetTransform = world->GetComponent<components::TransformComponent>(
                        ecs::Entity::FromID(camera.followTarget.entityId));
                    if (targetTransform)
                    {
                        sf::Vector2f targetPos(
                            targetTransform->position.x + camera.followTarget.offset.x,
                            targetTransform->position.y + camera.followTarget.offset.y
                        );
                        camera.Update(deltaTime, targetPos);
                    }
                }

                UpdateCameraShake(camera, deltaTime);
                ConstrainCamera(camera);

                camera.view.setCenter(camera.GetCenter());
                camera.view.setSize(camera.GetSize());
            }
        }
    }

    UpdateFadeEffect(deltaTime);

    if (m_hasActiveCamera && m_renderTarget)
    {
        auto* activeCam = world->GetComponent<components::CameraComponent>(m_activeCamera);
        if (activeCam)
        {
            ApplyViewToTarget(*activeCam);
        }
    }
}

void CameraSystem::OnRender(ecs::World* world, float alpha)
{
    (void)world;
    (void)alpha;
}

void CameraSystem::SetActiveCamera(ecs::Entity cameraEntity)
{
    m_activeCamera = cameraEntity;
    m_hasActiveCamera = true;
}

ecs::Entity CameraSystem::GetActiveCamera() const
{
    return m_activeCamera;
}

void CameraSystem::SetDefaultViewSize(const sf::Vector2f& size)
{
    m_defaultViewSize = size;
}

void CameraSystem::SetRenderTarget(sf::RenderTarget* target)
{
    m_renderTarget = target;
}

void CameraSystem::EnableScreenShake(bool enabled)
{
    if (!enabled)
    {
        m_shake.active = false;
        m_shake.intensity = 0.0f;
    }
}

void CameraSystem::Shake(float intensity, float duration)
{
    m_shake.intensity = intensity;
    m_shake.duration = duration;
    m_shake.elapsed = 0.0f;
    m_shake.active = true;
}

void CameraSystem::FadeIn(float duration)
{
    m_fade.duration = duration;
    m_fade.elapsed = 0.0f;
    m_fade.fadingIn = true;
    m_fade.active = true;
    m_fade.color = sf::Color::Black;
}

void CameraSystem::FadeOut(float duration)
{
    m_fade.duration = duration;
    m_fade.elapsed = 0.0f;
    m_fade.fadingIn = false;
    m_fade.active = true;
    m_fade.color = sf::Color::Black;
}

void CameraSystem::UpdateCameraFollow(components::CameraComponent& camera,
                                       ecs::World* world, float dt)
{
    if (!camera.IsFollowing())
        return;

    auto* targetTransform = world->GetComponent<components::TransformComponent>(
        ecs::Entity::FromID(camera.followTarget.entityId));
    if (!targetTransform)
        return;

    sf::Vector2f targetPos(
        targetTransform->position.x + camera.followTarget.offset.x,
        targetTransform->position.y + camera.followTarget.offset.y
    );

    camera.Update(dt, targetPos);
}

void CameraSystem::UpdateCameraShake(components::CameraComponent& camera, float dt)
{
    if (!m_shake.active)
        return;

    m_shake.elapsed += dt;
    if (m_shake.elapsed >= m_shake.duration)
    {
        m_shake.active = false;
        return;
    }

    float intensity = m_shake.intensity * (1.0f - m_shake.elapsed / m_shake.duration);
    float offsetX = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 2.0f * intensity;
    float offsetY = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 2.0f * intensity;

    sf::Vector2f center = camera.GetCenter();
    camera.SetCenter(center + sf::Vector2f(offsetX, offsetY));
}

void CameraSystem::ConstrainCamera(components::CameraComponent& camera)
{
    if (!camera.constrainToWorld)
        return;

    sf::Vector2f center = camera.GetCenter();
    sf::Vector2f halfSize = camera.GetSize() * 0.5f;

    float minX = camera.worldBounds.left + halfSize.x;
    float maxX = camera.worldBounds.left + camera.worldBounds.width - halfSize.x;
    float minY = camera.worldBounds.top + halfSize.y;
    float maxY = camera.worldBounds.top + camera.worldBounds.height - halfSize.y;

    if (camera.worldBounds.width > camera.GetSize().x)
    {
        center.x = std::clamp(center.x, minX, maxX);
    }
    if (camera.worldBounds.height > camera.GetSize().y)
    {
        center.y = std::clamp(center.y, minY, maxY);
    }

    camera.SetCenter(center);
}

void CameraSystem::ApplyViewToTarget(const components::CameraComponent& camera)
{
    if (m_renderTarget)
    {
        m_renderTarget->setView(camera.view);
    }
}

void CameraSystem::UpdateFadeEffect(float dt)
{
    if (!m_fade.active)
        return;

    m_fade.elapsed += dt;
    if (m_fade.elapsed >= m_fade.duration)
    {
        m_fade.active = false;
    }
}

} // namespace systems
} // namespace engine
