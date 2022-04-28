#include "UIComponent.h"
#include <algorithm>

namespace engine {
namespace components {

UIComponent::UIComponent()
    : widgetType(WidgetType::Panel)
    , anchor(UIAnchor::TopLeft)
    , offset(0.f, 0.f)
    , size(100.f, 50.f)
    , rect(0.f, 0.f, 100.f, 50.f)
    , backgroundColor(sf::Color(50, 50, 50, 200))
    , borderColor(sf::Color::White)
    , hoverColor(sf::Color(80, 80, 80, 220))
    , pressedColor(sf::Color(30, 30, 30, 200))
    , borderThickness(0.f)
    , cornerRadius(0.f)
    , zOrder(0)
{
}

void UIComponent::SetAnchor(UIAnchor a)
{
    anchor = a;
}

void UIComponent::SetOffset(const sf::Vector2f& off)
{
    offset = off;
}

void UIComponent::SetSize(const sf::Vector2f& sz)
{
    size = sf::Vector2f(std::max(1.0f, sz.x), std::max(1.0f, sz.y));
}

void UIComponent::SetRect(const sf::FloatRect& r)
{
    rect = r;
    size = sf::Vector2f(r.width, r.height);
    offset = sf::Vector2f(r.left, r.top);
}

void UIComponent::SetInteractable(bool interactable)
{
    state.interactable = interactable;
}

void UIComponent::SetDisabled(bool disabled)
{
    state.disabled = disabled;
    state.interactable = !disabled;
}

void UIComponent::SetVisible(bool visible)
{
    state.visible = visible;
}

void UIComponent::SetOnClick(std::function<void()> callback)
{
    onClick = std::move(callback);
}

void UIComponent::SetOnHover(std::function<void()> callback)
{
    onHover = std::move(callback);
}

void UIComponent::SetOnUnhover(std::function<void()> callback)
{
    onUnhover = std::move(callback);
}

void UIComponent::SetOnDrag(std::function<void()> callback)
{
    onDrag = std::move(callback);
}

void UIComponent::SimulateClick()
{
    if (onClick && !state.disabled && state.interactable) {
        onClick();
    }
}

void UIComponent::SimulateHover()
{
    if (!state.hovered && onHover) {
        state.hovered = true;
        onHover();
    }
}

void UIComponent::SimulateUnhover()
{
    if (state.hovered && onUnhover) {
        state.hovered = false;
        onUnhover();
    }
}

bool UIComponent::IsHovered() const
{
    return state.hovered;
}

bool UIComponent::IsPressed() const
{
    return state.pressed;
}

bool UIComponent::IsFocused() const
{
    return state.focused;
}

bool UIComponent::IsDisabled() const
{
    return state.disabled;
}

void UIComponent::SetColors(const sf::Color& bg, const sf::Color& border,
                            const sf::Color& hover, const sf::Color& pressed)
{
    backgroundColor = bg;
    borderColor = border;
    hoverColor = hover;
    pressedColor = pressed;
}

void UIComponent::SetBorder(float thickness, const sf::Color& color)
{
    borderThickness = std::max(0.0f, thickness);
    borderColor = color;
}

sf::Vector2f UIComponent::GetAnchorPosition(const sf::Vector2f& containerSize) const
{
    sf::Vector2f pos(0.f, 0.f);
    switch (anchor) {
        case UIAnchor::TopLeft:      break;
        case UIAnchor::TopCenter:    pos.x = (containerSize.x - size.x) / 2.f; break;
        case UIAnchor::TopRight:     pos.x = containerSize.x - size.x; break;
        case UIAnchor::MiddleLeft:   pos.y = (containerSize.y - size.y) / 2.f; break;
        case UIAnchor::Center:       pos = (containerSize - size) / 2.f; break;
        case UIAnchor::MiddleRight:  pos.x = containerSize.x - size.x; pos.y = (containerSize.y - size.y) / 2.f; break;
        case UIAnchor::BottomLeft:   pos.y = containerSize.y - size.y; break;
        case UIAnchor::BottomCenter: pos.x = (containerSize.x - size.x) / 2.f; pos.y = containerSize.y - size.y; break;
        case UIAnchor::BottomRight:  pos = containerSize - size; break;
    }
    return pos + offset;
}

void UIComponent::RecalculateRect(const sf::Vector2f& containerSize)
{
    sf::Vector2f pos = GetAnchorPosition(containerSize);
    rect = sf::FloatRect(pos.x, pos.y, size.x, size.y);
}

} // namespace components
} // namespace engine
