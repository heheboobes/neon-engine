#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <optional>

namespace engine::ui {

enum class Anchor {
    TopLeft, TopCenter, TopRight,
    CenterLeft, Center, CenterRight,
    BottomLeft, BottomCenter, BottomRight
};

enum class Visibility {
    Visible, Hidden, Collapsed
};

struct Padding {
    float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
    sf::Vector2f topLeft() const { return {left, top}; }
    sf::Vector2f bottomRight() const { return {right, bottom}; }
};

struct Margin {
    float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
};

class Widget : public std::enable_shared_from_this<Widget> {
public:
    using Ptr = std::shared_ptr<Widget>;
    using EventCallback = std::function<void(Widget&)>;
    using KeyCallback = std::function<void(Widget&, const sf::Event::KeyEvent&)>;
    using ClickCallback = std::function<void(Widget&)>;

    Widget();
    virtual ~Widget() = default;

    virtual void update(float dt);
    virtual bool handleEvent(const sf::Event& event);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    virtual sf::FloatRect getBounds() const;
    virtual bool contains(sf::Vector2f point) const;

    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const;

    void setSize(sf::Vector2f size);
    sf::Vector2f getSize() const;

    void setAnchor(Anchor anchor);
    Anchor getAnchor() const;

    void setMargin(Margin margin);
    Margin getMargin() const;

    void setPadding(Padding padding);
    Padding getPadding() const;

    void setVisibility(Visibility vis);
    Visibility getVisibility() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setParent(Widget* parent);
    Widget* getParent() const;

    void addChild(Ptr child);
    void removeChild(Ptr child);
    void clearChildren();
    const std::vector<Ptr>& getChildren() const;

    void onLayout();

    EventCallback onClick;
    EventCallback onHover;
    EventCallback onPress;
    EventCallback onRelease;
    KeyCallback onKey;
    ClickCallback onChange;

protected:
    sf::Vector2f position_;
    sf::Vector2f size_;
    Anchor anchor_ = Anchor::TopLeft;
    Margin margin_;
    Padding padding_;
    Visibility visibility_ = Visibility::Visible;
    bool enabled_ = true;
    bool hovered_ = false;
    bool pressed_ = false;

    Widget* parent_ = nullptr;
    std::vector<Ptr> children_;

    void propagateEvent(const sf::Event& event, bool& handled);
    void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
    void layoutChildren();
    virtual void onLayoutSelf();
};

} // namespace engine::ui
