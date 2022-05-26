#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/UIComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <functional>

namespace engine {
namespace systems {

struct UIHitResult {
    ecs::Entity entity;
    components::UIComponent* component;
    sf::Vector2f localPoint;
    bool hit;
};

class UISystem : public ecs::ISystem {
public:
    UISystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnRender(ecs::World* world, float alpha) override;

    void ProcessEvent(const sf::Event& event, const sf::Vector2f& mousePos);
    void SetContainerSize(const sf::Vector2f& size);
    sf::Vector2f GetContainerSize() const;

    UIHitResult HitTest(const sf::Vector2f& point) const;
    void SetFocus(ecs::Entity entity);
    void ClearFocus();

    void SetGlobalOpacity(float opacity);
    void SetScale(float scale);

private:
    void UpdateRectLayouts(ecs::World* world);
    void ProcessHover(const sf::Vector2f& mousePos);
    void ProcessClick(const sf::Vector2f& mousePos, bool pressed);
    void DispatchClick(ecs::Entity entity, components::UIComponent& ui);

    void CollectInteractables(ecs::World* world);

    std::vector<std::pair<ecs::Entity, components::UIComponent*>> m_interactables;
    sf::Vector2f m_containerSize;
    ecs::Entity m_focusedEntity;
    ecs::Entity m_hoveredEntity;
    float m_globalOpacity;
    float m_scale;
    bool m_mouseDown;
};

} // namespace systems
} // namespace engine
