#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <unordered_map>

namespace engine {
namespace systems {

class CameraSystem : public ecs::ISystem {
public:
    CameraSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnRender(ecs::World* world, float alpha) override;

    void SetActiveCamera(ecs::Entity cameraEntity);
    ecs::Entity GetActiveCamera() const;
    void SetDefaultViewSize(const sf::Vector2f& size);
    void SetRenderTarget(sf::RenderTarget* target);
    void EnableScreenShake(bool enabled);

    void Shake(float intensity, float duration);
    void FadeIn(float duration);
    void FadeOut(float duration);

private:
    void UpdateCameraFollow(components::CameraComponent& camera,
                            ecs::World* world, float dt);
    void UpdateCameraShake(components::CameraComponent& camera, float dt);
    void ConstrainCamera(components::CameraComponent& camera);
    void ApplyViewToTarget(const components::CameraComponent& camera);
    void UpdateFadeEffect(float dt);

    ecs::Entity m_activeCamera;
    sf::RenderTarget* m_renderTarget;
    sf::Vector2f m_defaultViewSize;

    struct ScreenShake {
        float intensity;
        float duration;
        float elapsed;
        bool active;
    } m_shake;

    struct FadeEffect {
        float duration;
        float elapsed;
        bool fadingIn;
        bool active;
        sf::Color color;
    } m_fade;

    bool m_hasActiveCamera;
};

} // namespace systems
} // namespace engine
// v2: Fade-to-black effect
