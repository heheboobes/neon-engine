#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include "../../engine/ecs/Component.h"
#include <string>
#include <functional>

namespace engine {
namespace components {

enum class WidgetType {
    None,
    Button,
    Label,
    Panel,
    Slider,
    TextBox,
    CheckBox,
    Image,
    ProgressBar,
    Dropdown
};

enum class UIAnchor {
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    Center,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

struct UIState {
    bool hovered;
    bool pressed;
    bool focused;
    bool disabled;
    bool visible;
    bool interactable;

    UIState()
        : hovered(false), pressed(false), focused(false)
        , disabled(false), visible(true), interactable(true)
    {}
};

class UIComponent : public ecs::Component<UIComponent> {
public:
    WidgetType widgetType;
    UIAnchor anchor;
    sf::Vector2f offset;
    sf::Vector2f size;
    sf::FloatRect rect;

    UIState state;
    std::string id;
    std::string tooltip;

    sf::Color backgroundColor;
    sf::Color borderColor;
    sf::Color hoverColor;
    sf::Color pressedColor;

    float borderThickness;
    float cornerRadius;
    int zOrder;

    std::function<void()> onClick;
    std::function<void()> onHover;
    std::function<void()> onUnhover;
    std::function<void()> onDrag;

    UIComponent();

    void SetAnchor(UIAnchor a);
    void SetOffset(const sf::Vector2f& off);
    void SetSize(const sf::Vector2f& sz);
    void SetRect(const sf::FloatRect& r);

    void SetInteractable(bool interactable);
    void SetDisabled(bool disabled);
    void SetVisible(bool visible);

    void SetOnClick(std::function<void()> callback);
    void SetOnHover(std::function<void()> callback);
    void SetOnUnhover(std::function<void()> callback);
    void SetOnDrag(std::function<void()> callback);

    void SimulateClick();
    void SimulateHover();
    void SimulateUnhover();

    bool IsHovered() const;
    bool IsPressed() const;
    bool IsFocused() const;
    bool IsDisabled() const;

    void SetColors(const sf::Color& bg, const sf::Color& border,
                   const sf::Color& hover, const sf::Color& pressed);
    void SetBorder(float thickness, const sf::Color& color);

    sf::Vector2f GetAnchorPosition(const sf::Vector2f& containerSize) const;
    void RecalculateRect(const sf::Vector2f& containerSize);
};

} // namespace components
} // namespace engine
// v2: Added interactable state
