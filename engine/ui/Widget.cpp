#include "Widget.h"
#include <algorithm>

namespace engine::ui {

Widget::Widget()
    : position_(0.f, 0.f)
    , size_(100.f, 30.f)
{
}

void Widget::update(float dt) {
    if (visibility_ != Visibility::Visible) return;
    onLayout();
    for (auto& child : children_) {
        child->update(dt);
    }
}

bool Widget::handleEvent(const sf::Event& event) {
    if (visibility_ != Visibility::Visible || !enabled_) return false;
    bool handled = false;
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if ((*it)->handleEvent(event)) {
            handled = true;
            break;
        }
    }
    if (!handled) {
        propagateEvent(event, handled);
    }
    return handled;
}

void Widget::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (visibility_ != Visibility::Visible) return;
    states.transform.translate(position_);
    drawChildren(target, states);
}

sf::FloatRect Widget::getBounds() const {
    return sf::FloatRect(position_, size_);
}

bool Widget::contains(sf::Vector2f point) const {
    return getBounds().contains(point);
}

void Widget::setPosition(sf::Vector2f pos) { position_ = pos; }
sf::Vector2f Widget::getPosition() const { return position_; }

void Widget::setSize(sf::Vector2f size) { size_ = size; }
sf::Vector2f Widget::getSize() const { return size_; }

void Widget::setAnchor(Anchor anchor) { anchor_ = anchor; }
Anchor Widget::getAnchor() const { return anchor_; }

void Widget::setMargin(Margin margin) { margin_ = margin; }
Margin Widget::getMargin() const { return margin_; }

void Widget::setPadding(Padding padding) { padding_ = padding; }
Padding Widget::getPadding() const { return padding_; }

void Widget::setVisibility(Visibility vis) { visibility_ = vis; }
Visibility Widget::getVisibility() const { return visibility_; }

void Widget::setEnabled(bool enabled) { enabled_ = enabled; }
bool Widget::isEnabled() const { return enabled_; }

void Widget::setParent(Widget* parent) { parent_ = parent; }
Widget* Widget::getParent() const { return parent_; }

void Widget::addChild(Ptr child) {
    child->setParent(this);
    children_.push_back(std::move(child));
}

void Widget::removeChild(Ptr child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void Widget::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

const std::vector<Widget::Ptr>& Widget::getChildren() const {
    return children_;
}

void Widget::onLayout() {
    layoutChildren();
    onLayoutSelf();
}

void Widget::propagateEvent(const sf::Event& event, bool& handled) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mouse(static_cast<float>(event.mouseMove.x),
                           static_cast<float>(event.mouseMove.y));
        bool inside = contains(mouse);
        if (inside && !hovered_) {
            hovered_ = true;
            if (onHover) onHover(*this);
        } else if (!inside && hovered_) {
            hovered_ = false;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        if (contains(mouse) && event.mouseButton.button == sf::Mouse::Left) {
            pressed_ = true;
            if (onPress) onPress(*this);
            handled = true;
        }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        if (pressed_) {
            pressed_ = false;
            if (onRelease) onRelease(*this);
            if (contains(mouse) && onClick) onClick(*this);
            handled = true;
        }
    }
    if (event.type == sf::Event::KeyPressed && onKey) {
        onKey(*this, event.key);
    }
}

void Widget::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& child : children_) {
        child->draw(target, states);
    }
}

void Widget::layoutChildren() {
    sf::Vector2f cursor(padding_.left, padding_.top);
    for (auto& child : children_) {
        if (child->getVisibility() == Visibility::Collapsed) continue;
        child->setPosition(cursor);
        cursor.y += child->getSize().y + child->getMargin().bottom;
    }
}

void Widget::onLayoutSelf() {
}

} // namespace engine::ui
