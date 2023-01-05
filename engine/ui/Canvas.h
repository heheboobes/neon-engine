#pragma once

#include "Widget.h"

namespace engine::ui {

class Canvas : public Widget {
public:
    Canvas();
    explicit Canvas(sf::Vector2f size);
    ~Canvas() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setClearColor(sf::Color color);
    sf::Color getClearColor() const;

    void setTargetSize(sf::Vector2f size);
    sf::Vector2f getTargetSize() const;

    Widget* hitTest(sf::Vector2f point);
    void requestFocus(Ptr widget);
    void clearFocus();

private:
    sf::Color clearColor_{30, 30, 30, 255};
    Widget::Ptr focusedWidget_;

    void layoutRoot();
};

} // namespace engine::ui
