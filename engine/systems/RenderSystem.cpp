#include "RenderSystem.h"
#include "../ecs/World.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cassert>

namespace engine {
namespace systems {

RenderSystem::RenderSystem()
    : m_target(nullptr)
    , m_clearColor(sf::Color::Black)
    , m_viewportLayer(0)
    , m_sortByDepth(true)
    , m_hasCamera(false)
{
    SetName("RenderSystem");
    SetPriority(100);
    Require<components::SpriteComponent, components::TransformComponent>();
}

void RenderSystem::SetTarget(sf::RenderTarget* target)
{
    m_target = target;
}

void RenderSystem::SetViewportLayer(int32_t layer)
{
    m_viewportLayer = layer;
}

void RenderSystem::ClearColor(const sf::Color& color)
{
    m_clearColor = color;
}

void RenderSystem::SetSortByDepth(bool enabled)
{
    m_sortByDepth = enabled;
}

void RenderSystem::SetCamera(ecs::Entity cameraEntity)
{
    m_cameraEntity = cameraEntity;
    m_hasCamera = true;
}

void RenderSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    (void)deltaTime;
    CollectRenderables(world);
    SortRenderables();
}

void RenderSystem::OnRender(ecs::World* world, float alpha)
{
    (void)world;
    (void)alpha;

    if (!m_target)
        return;

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;

    if (m_hasCamera)
    {
        auto* camera = world->GetComponent<components::CameraComponent>(m_cameraEntity);
        if (camera && camera->active)
        {
            m_target->setView(camera->view);
        }
    }

    SubmitDrawCalls(*m_target);
    m_renderQueue.clear();
}

void RenderSystem::CollectRenderables(ecs::World* world)
{
    m_renderQueue.clear();

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* sprites = static_cast<components::SpriteComponent*>(
                chunk.GetComponentData(components::SpriteComponent::GetStaticTypeID()));
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!sprites || !transforms)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                if (!sprites[i].visible || !sprites[i].HasTexture())
                    continue;

                RenderCommand cmd;
                cmd.entity = chunk.GetEntity(i);
                cmd.sprite = &sprites[i];
                cmd.transform = &transforms[i];
                cmd.layer = RenderLayer(0, 0, transforms[i].position.z);
                m_renderQueue.push_back(cmd);
            }
        }
    }
}

void RenderSystem::SortRenderables()
{
    if (!m_sortByDepth)
        return;

    std::sort(m_renderQueue.begin(), m_renderQueue.end(),
        [](const RenderCommand& a, const RenderCommand& b) {
            if (a.layer.layer != b.layer.layer)
                return a.layer.layer < b.layer.layer;
            return a.layer.depth < b.layer.depth;
        });
}

void RenderSystem::SubmitDrawCalls(sf::RenderTarget& target)
{
    target.clear(m_clearColor);

    for (const auto& cmd : m_renderQueue)
    {
        sf::Sprite drawSprite = cmd.sprite->sprite;
        drawSprite.setPosition(cmd.transform->position.x, cmd.transform->position.y);
        drawSprite.setRotation(cmd.transform->rotation.z);
        drawSprite.setScale(
            cmd.transform->scale.x * (cmd.sprite->flipX ? -1.0f : 1.0f),
            cmd.transform->scale.y * (cmd.sprite->flipY ? -1.0f : 1.0f)
        );

        sf::Color spriteColor = drawSprite.getColor();
        spriteColor.a = cmd.sprite->opacity;
        drawSprite.setColor(spriteColor);

        target.draw(drawSprite);
    }
}

} // namespace systems
} // namespace engine
// v2: Added layer mask filtering
// v3: Bottleneck profiling
