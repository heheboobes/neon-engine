#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/AnimationComponent.h"
#include "../components/SpriteComponent.h"
#include <unordered_map>

namespace engine {
namespace systems {

struct AnimationEvent {
    std::string animationName;
    int frameIndex;
    ecs::Entity entity;
};

class AnimationSystem : public ecs::ISystem {
public:
    AnimationSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;

    void SetGlobalSpeedMultiplier(float multiplier);
    float GetGlobalSpeedMultiplier() const;
    void PauseAll();
    void ResumeAll();

    using FrameChangeCallback = std::function<void(const AnimationEvent&)>;
    void OnFrameChange(FrameChangeCallback callback);

private:
    void UpdateAnimation(ecs::World* world, ecs::Entity entity,
                         components::AnimationComponent& anim,
                         components::SpriteComponent* sprite,
                         float dt);

    void ApplyFrameToSprite(const components::AnimationFrame& frame,
                            components::SpriteComponent& sprite);

    void DispatchFrameEvent(const AnimationEvent& event);

    float m_globalSpeedMultiplier;
    bool m_globalPaused;
    std::vector<FrameChangeCallback> m_frameCallbacks;
};

} // namespace systems
} // namespace engine
