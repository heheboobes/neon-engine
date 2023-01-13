#include "Panel.h"

namespace engine::ui {

Panel::Panel()
    : Widget()
{
    setSize({300.f, 200.f});
    background_.setSize(getSize());
    background_.setFillColor(backgroundColor_);
    border_.setSize(getSize());
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineColor(borderColor_);
    border_.setOutlineThickness(borderThickness_);
}

Panel::Panel(LayoutMode layoutMode)
    : Widget()
    , layoutMode_(layoutMode)
{
    setSize({300.f, 200.f});
    background_.setSize(getSize());
    background_.setFillColor(backgroundColor_);
    border_.setSize(getSize());
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineColor(borderColor_);
    border_.setOutlineThickness(borderThickness_);
}

void Panel::update(float dt) {
    if (getVisibility() != Visibility::Visible) return;
    arrangeChildren();
    Widget::update(dt);
}

bool Panel::handleEvent(const sf::Event& event) {
    if (!isEnabled()) return false;
    return Widget::handleEvent(event);
}

void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());
    target.draw(background_, states);
    if (borderThickness_ > 0.f) {
        target.draw(border_, states);
    }
    Widget::draw(target, states);
}

void Panel::setLayoutMode(LayoutMode mode) {
    layoutMode_ = mode;
    arrangeChildren();
}

LayoutMode Panel::getLayoutMode() const {
    return layoutMode_;
}

void Panel::setBackgroundColor(sf::Color color) {
    backgroundColor_ = color;
    background_.setFillColor(backgroundColor_);
}

sf::Color Panel::getBackgroundColor() const {
    return backgroundColor_;
}

void Panel::setBorderColor(sf::Color color) {
    borderColor_ = color;
    border_.setOutlineColor(borderColor_);
}

sf::Color Panel::getBorderColor() const {
    return borderColor_;
}

void Panel::setBorderThickness(float thickness) {
    borderThickness_ = thickness;
    border_.setOutlineThickness(borderThickness_);
}

float Panel::getBorderThickness() const {
    return borderThickness_;
}

void Panel::setGridColumns(int columns) {
    gridColumns_ = std::max(1, columns);
    arrangeChildren();
}

int Panel::getGridColumns() const {
    return gridColumns_;
}

void Panel::setSpacing(float spacing) {
    spacing_ = spacing;
    arrangeChildren();
}

float Panel::getSpacing() const {
    return spacing_;
}

void Panel::setPadding(float all) {
    Padding p;
    p.left = p.top = p.right = p.bottom = all;
    Widget::setPadding(p);
    arrangeChildren();
}

void Panel::setPadding(float horizontal, float vertical) {
    Padding p;
    p.left = p.right = horizontal;
    p.top = p.bottom = vertical;
    Widget::setPadding(p);
    arrangeChildren();
}

void Panel::arrangeChildren() {
    switch (layoutMode_) {
        case LayoutMode::Vertical:   arrangeVertical();   break;
        case LayoutMode::Horizontal: arrangeHorizontal(); break;
        case LayoutMode::Grid:       arrangeGrid();       break;
    }
    background_.setSize(getSize());
    border_.setSize(getSize());
}

void Panel::arrangeVertical() {
    float y = getPadding().top;
    float contentWidth = getSize().x - getPadding().left - getPadding().right;
    for (auto& child : getChildren()) {
        if (child->getVisibility() == Visibility::Collapsed) continue;
        auto childSize = child->getSize();
        float x = getPadding().left;
        switch (child->getAnchor()) {
            case Anchor::TopCenter:
            case Anchor::Center:
                x = getPadding().left + (contentWidth - childSize.x) / 2.f;
                break;
            case Anchor::TopRight:
            case Anchor::CenterRight:
                x = getSize().x - getPadding().right - childSize.x;
                break;
            default: break;
        }
        child->setPosition({x, y});
        y += childSize.y + spacing_;
    }
}

void Panel::arrangeHorizontal() {
    float x = getPadding().left;
    float contentHeight = getSize().y - getPadding().top - getPadding().bottom;
    for (auto& child : getChildren()) {
        if (child->getVisibility() == Visibility::Collapsed) continue;
        auto childSize = child->getSize();
        float y = getPadding().top;
        switch (child->getAnchor()) {
            case Anchor::CenterLeft:
            case Anchor::Center:
                y = getPadding().top + (contentHeight - childSize.y) / 2.f;
                break;
            case Anchor::BottomLeft:
            case Anchor::BottomCenter:
                y = getSize().y - getPadding().bottom - childSize.y;
                break;
            default: break;
        }
        child->setPosition({x, y});
        x += childSize.x + spacing_;
    }
}

void Panel::arrangeGrid() {
    int cols = std::max(1, gridColumns_);
    float x = getPadding().left;
    float y = getPadding().top;
    int col = 0;
    float contentWidth = getSize().x - getPadding().left - getPadding().right;
    float cellWidth = (contentWidth - (cols - 1) * spacing_) / cols;

    for (auto& child : getChildren()) {
        if (child->getVisibility() == Visibility::Collapsed) continue;
        child->setPosition({x, y});
        if (child->getSize().x <= 0.f) {
            child->setSize({cellWidth, child->getSize().y});
        }
        col++;
        if (col >= cols) {
            col = 0;
            x = getPadding().left;
            y += child->getSize().y + spacing_;
        } else {
            x += cellWidth + spacing_;
        }
    }
}

} // namespace engine::ui
