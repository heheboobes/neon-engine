#include "UISystem.h"
#include "../ecs/World.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace systems {

UISystem::UISystem()
    : m_containerSize(1280.0f, 720.0f)
    , m_globalOpacity(1.0f)
    , m_scale(1.0f)
    , m_mouseDown(false)
{
    SetName("UISystem");
    SetPriority(80);
    Require<components::UIComponent>();
}

void UISystem::OnUpdate(ecs::World* world, float deltaTime)
{
    (void)deltaTime;
    UpdateRectLayouts(world);
}

void UISystem::OnRender(ecs::World* world, float alpha)
{
    (void)world;
    (void)alpha;
}

void UISystem::ProcessEvent(const sf::Event& event, const sf::Vector2f& mousePos)
{
    switch (event.type)
    {
    case sf::Event::MouseMoved:
        ProcessHover(mousePos);
        break;
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            m_mouseDown = true;
            ProcessClick(mousePos, true);
        }
        break;
    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            m_mouseDown = false;
            ProcessClick(mousePos, false);
        }
        break;
    default:
        break;
    }
}

void UISystem::SetContainerSize(const sf::Vector2f& size)
{
    m_containerSize = size;
}

sf::Vector2f UISystem::GetContainerSize() const
{
    return m_containerSize;
}

UIHitResult UISystem::HitTest(const sf::Vector2f& point) const
{
    for (const auto& [entity, ui] : m_interactables)
    {
        if (!ui->state.visible || !ui->state.interactable)
            continue;

        sf::FloatRect rect(
            ui->rect.left * m_scale + ui->offset.x,
            ui->rect.top * m_scale + ui->offset.y,
            ui->rect.width * m_scale,
            ui->rect.height * m_scale
        );

        if (rect.contains(point))
        {
            UIHitResult result;
            result.entity = entity;
            result.component = const_cast<components::UIComponent*>(ui);
            result.localPoint = sf::Vector2f(point.x - rect.left, point.y - rect.top);
            result.hit = true;
            return result;
        }
    }

    UIHitResult result;
    result.hit = false;
    return result;
}

void UISystem::SetFocus(ecs::Entity entity)
{
    m_focusedEntity = entity;
}

void UISystem::ClearFocus()
{
    m_focusedEntity = ecs::Entity::Null();
}

void UISystem::SetGlobalOpacity(float opacity)
{
    m_globalOpacity = std::clamp(opacity, 0.0f, 1.0f);
}

void UISystem::SetScale(float scale)
{
    m_scale = std::max(0.1f, scale);
}

void UISystem::UpdateRectLayouts(ecs::World* world)
{
    m_interactables.clear();

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* uis = static_cast<components::UIComponent*>(
                chunk.GetComponentData(components::UIComponent::GetStaticTypeID()));

            if (!uis)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& ui = uis[i];
                sf::Vector2f anchorPos = ui.GetAnchorPosition(m_containerSize);
                ui.rect.left = anchorPos.x + ui.offset.x;
                ui.rect.top = anchorPos.y + ui.offset.y;
                ui.rect.width = ui.size.x;
                ui.rect.height = ui.size.y;

                if (ui.state.interactable && ui.state.visible)
                {
                    m_interactables.emplace_back(chunk.GetEntity(i), &ui);
                }
            }
        }
    }

    std::sort(m_interactables.begin(), m_interactables.end(),
        [](const auto& a, const auto& b) {
            return a.second->zOrder < b.second->zOrder;
        });
}

void UISystem::ProcessHover(const sf::Vector2f& mousePos)
{
    ecs::Entity newHovered = ecs::Entity::Null();

    for (const auto& [entity, ui] : m_interactables)
    {
        sf::FloatRect rect(
            ui->rect.left * m_scale,
            ui->rect.top * m_scale,
            ui->rect.width * m_scale,
            ui->rect.height * m_scale
        );

        if (rect.contains(mousePos))
        {
            newHovered = entity;
            break;
        }
    }

    if (newHovered != m_hoveredEntity)
    {
        if (m_hoveredEntity.IsValid())
        {
            for (const auto& [entity, ui] : m_interactables)
            {
                if (entity == m_hoveredEntity)
                {
                    ui->state.hovered = false;
                    if (ui->onUnhover)
                        ui->onUnhover();
                    break;
                }
            }
        }

        m_hoveredEntity = newHovered;

        if (m_hoveredEntity.IsValid())
        {
            for (const auto& [entity, ui] : m_interactables)
            {
                if (entity == m_hoveredEntity)
                {
                    ui->state.hovered = true;
                    if (ui->onHover)
                        ui->onHover();
                    break;
                }
            }
        }
    }
}

void UISystem::ProcessClick(const sf::Vector2f& mousePos, bool pressed)
{
    if (!pressed && m_hoveredEntity.IsValid())
    {
        m_focusedEntity = m_hoveredEntity;
        DispatchClick(m_hoveredEntity,
                      *std::find_if(m_interactables.begin(), m_interactables.end(),
                          [this](const auto& pair) { return pair.first == m_hoveredEntity; })->second);
        return;
    }

    UIHitResult hit = HitTest(mousePos);
    if (hit.hit && pressed)
    {
        hit.component->state.pressed = true;
        m_focusedEntity = hit.entity;
    }
    else if (!pressed)
    {
        for (const auto& [entity, ui] : m_interactables)
        {
            if (ui->state.pressed)
            {
                ui->state.pressed = false;
            }
        }
    }
}

void UISystem::DispatchClick(ecs::Entity entity, components::UIComponent& ui)
{
    (void)entity;
    if (ui.onClick)
        ui.onClick();
}

void UISystem::CollectInteractables(ecs::World* world)
{
    (void)world;
}

} // namespace systems
} // namespace engine
