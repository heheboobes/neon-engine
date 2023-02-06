#include "Canvas.h"

namespace engine::ui {

Canvas::Canvas()
    : Widget()
{
    setSize({800.f, 600.f});
}

Canvas::Canvas(sf::Vector2f size)
    : Widget()
{
    setSize(size);
}

void Canvas::update(float dt) {
    if (getVisibility() != Visibility::Visible) return;
    layoutRoot();
    Widget::update(dt);
}

bool Canvas::handleEvent(const sf::Event& event) {
    if (getVisibility() != Visibility::Visible) return false;
    if (event.type == sf::Event::Resized) {
        setSize(static_cast<sf::Vector2f>(sf::Vector2i(event.size.width, event.size.height)));
        layoutRoot();
    }
    if (event.type == sf::Event::MouseButtonPressed && focusedWidget_) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        auto* hit = hitTest(mouse);
        if (hit != focusedWidget_.get()) {
            clearFocus();
        }
    }
    return Widget::handleEvent(event);
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    sf::RectangleShape bg(getSize());
    bg.setFillColor(clearColor_);
    target.draw(bg, states);
    Widget::draw(target, states);
}

void Canvas::setClearColor(sf::Color color) {
    clearColor_ = color;
}

sf::Color Canvas::getClearColor() const {
    return clearColor_;
}

void Canvas::setTargetSize(sf::Vector2f size) {
    setSize(size);
    layoutRoot();
}

sf::Vector2f Canvas::getTargetSize() const {
    return getSize();
}

Widget* Canvas::hitTest(sf::Vector2f point) {
    if (!getBounds().contains(point)) return nullptr;
    for (auto it = getChildren().rbegin(); it != getChildren().rend(); ++it) {
        auto* child = (*it)->contains(point - getPosition()) ? it->get() : nullptr;
        if (child) return child;
    }
    return this;
}

void Canvas::requestFocus(Ptr widget) {
    if (focusedWidget_) {
        focusedWidget_->onChange(*focusedWidget_);
    }
    focusedWidget_ = widget;
}

void Canvas::clearFocus() {
    if (focusedWidget_) {
        focusedWidget_->onChange(*focusedWidget_);
        focusedWidget_.reset();
    }
}

void Canvas::layoutRoot() {
    for (auto& child : getChildren()) {
        auto childSize = child->getSize();
        auto parentSize = getSize();
        switch (child->getAnchor()) {
            case Anchor::TopCenter:
                child->setPosition({(parentSize.x - childSize.x) / 2.f, 0.f});
                break;
            case Anchor::TopRight:
                child->setPosition({parentSize.x - childSize.x, 0.f});
                break;
            case Anchor::CenterLeft:
                child->setPosition({0.f, (parentSize.y - childSize.y) / 2.f});
                break;
            case Anchor::Center:
                child->setPosition({(parentSize.x - childSize.x) / 2.f,
                                    (parentSize.y - childSize.y) / 2.f});
                break;
            case Anchor::CenterRight:
                child->setPosition({parentSize.x - childSize.x,
                                    (parentSize.y - childSize.y) / 2.f});
                break;
            case Anchor::BottomLeft:
                child->setPosition({0.f, parentSize.y - childSize.y});
                break;
            case Anchor::BottomCenter:
                child->setPosition({(parentSize.x - childSize.x) / 2.f,
                                    parentSize.y - childSize.y});
                break;
            case Anchor::BottomRight:
                child->setPosition({parentSize.x - childSize.x,
                                    parentSize.y - childSize.y});
                break;
            default: break;
        }
    }
}

} // namespace engine::ui

