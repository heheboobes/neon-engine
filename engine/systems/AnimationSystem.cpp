#include "AnimationSystem.h"
#include "../ecs/World.h"

namespace engine {
namespace systems {

AnimationSystem::AnimationSystem()
    : m_globalSpeedMultiplier(1.0f)
    , m_globalPaused(false)
{
    SetName("AnimationSystem");
    SetPriority(30);
    Require<components::AnimationComponent>();
}

void AnimationSystem::SetGlobalSpeedMultiplier(float multiplier)
{
    m_globalSpeedMultiplier = std::max(0.0f, multiplier);
}

float AnimationSystem::GetGlobalSpeedMultiplier() const
{
    return m_globalSpeedMultiplier;
}

void AnimationSystem::PauseAll()
{
    m_globalPaused = true;
}

void AnimationSystem::ResumeAll()
{
    m_globalPaused = false;
}

void AnimationSystem::OnFrameChange(FrameChangeCallback callback)
{
    m_frameCallbacks.push_back(std::move(callback));
}

void AnimationSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    if (m_globalPaused || deltaTime <= 0.0f)
        return;

    float adjustedDt = deltaTime * m_globalSpeedMultiplier;

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* anims = static_cast<components::AnimationComponent*>(
                chunk.GetComponentData(components::AnimationComponent::GetStaticTypeID()));

            if (!anims)
                continue;

            auto* sprites = static_cast<components::SpriteComponent*>(
                chunk.GetComponentData(components::SpriteComponent::GetStaticTypeID()));

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& anim = anims[i];
                if (!anim.playing || anim.frames.empty())
                    continue;

                int previousFrame = anim.currentFrameIndex;
                anim.Update(adjustedDt);

                if (sprites && anim.currentFrameIndex < static_cast<int>(anim.frames.size()))
                {
                    ApplyFrameToSprite(anim.frames[anim.currentFrameIndex], *sprites);
                }

                if (anim.currentFrameIndex != previousFrame)
                {
                    AnimationEvent event;
                    event.animationName = anim.animationName;
                    event.frameIndex = anim.currentFrameIndex;
                    event.entity = chunk.GetEntity(i);
                    DispatchFrameEvent(event);
                }

                if (anim.finished)
                {
                    AnimationEvent event;
                    event.animationName = anim.animationName;
                    event.frameIndex = -1;
                    event.entity = chunk.GetEntity(i);
                    DispatchFrameEvent(event);
                }
            }
        }
    }
}

void AnimationSystem::UpdateAnimation(ecs::World* world, ecs::Entity entity,
                                       components::AnimationComponent& anim,
                                       components::SpriteComponent* sprite,
                                       float dt)
{
    (void)world;
    (void)entity;

    if (!anim.playing || anim.frames.empty())
        return;

    anim.Update(dt);

    if (sprite && anim.currentFrameIndex < static_cast<int>(anim.frames.size()))
    {
        ApplyFrameToSprite(anim.frames[anim.currentFrameIndex], *sprite);
    }
}

void AnimationSystem::ApplyFrameToSprite(const components::AnimationFrame& frame,
                                          components::SpriteComponent& sprite)
{
    sprite.SetTextureRect(frame.rect);
}

void AnimationSystem::DispatchFrameEvent(const AnimationEvent& event)
{
    for (auto& callback : m_frameCallbacks)
    {
        callback(event);
    }
}

} // namespace systems
} // namespace engine
