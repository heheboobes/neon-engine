#include "Button.h"

namespace engine::ui {

Button::Button()
    : Widget()
{
    setSize({120.f, 32.f});
    updateAppearance();
    centerText();
}

Button::Button(const std::string& text)
    : Widget()
    , text_(text)
{
    setSize({120.f, 32.f});
    sfText_.setString(text_);
    updateAppearance();
    centerText();
}

void Button::update(float dt) {
    Widget::update(dt);
}

bool Button::handleEvent(const sf::Event& event) {
    if (!isEnabled()) return false;
    bool handled = false;

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mouse(static_cast<float>(event.mouseMove.x),
                           static_cast<float>(event.mouseMove.y));
        if (contains(mouse)) {
            setState(ButtonState::Hovered);
        } else {
            setState(ButtonState::Normal);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        if (contains(mouse) && event.mouseButton.button == sf::Mouse::Left) {
            setState(ButtonState::Pressed);
            handled = true;
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        if (state_ == ButtonState::Pressed) {
            if (contains(mouse)) {
                if (onClicked_) onClicked_();
                if (onClick) onClick(*this);
                setState(ButtonState::Hovered);
            } else {
                setState(ButtonState::Normal);
            }
            handled = true;
        }
    }

    return handled;
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());
    target.draw(background_, states);
    target.draw(sfText_, states);
    Widget::draw(target, states);
}

void Button::setText(const std::string& text) {
    text_ = text;
    sfText_.setString(text_);
    centerText();
}

const std::string& Button::getText() const {
    return text_;
}

void Button::setFont(const sf::Font& font) {
    sfText_.setFont(font);
    centerText();
}

void Button::setFontSize(unsigned int size) {
    fontSize_ = size;
    sfText_.setCharacterSize(fontSize_);
    centerText();
}

unsigned int Button::getFontSize() const {
    return fontSize_;
}

void Button::setTextColor(sf::Color color) {
    textColor_ = color;
    updateAppearance();
}

void Button::setNormalColor(sf::Color color) {
    normalColor_ = color;
    updateAppearance();
}

void Button::setHoverColor(sf::Color color) {
    hoverColor_ = color;
    updateAppearance();
}

void Button::setPressedColor(sf::Color color) {
    pressedColor_ = color;
    updateAppearance();
}

void Button::setDisabledColor(sf::Color color) {
    disabledColor_ = color;
    updateAppearance();
}

void Button::setCornerRadius(float radius) {
    cornerRadius_ = radius;
}

float Button::getCornerRadius() const {
    return cornerRadius_;
}

ButtonState Button::getState() const {
    return state_;
}

void Button::setState(ButtonState state) {
    state_ = state;
    updateAppearance();
}

void Button::setOnClicked(ClickedCallback callback) {
    onClicked_ = std::move(callback);
}

sf::FloatRect Button::getTextBounds() const {
    return sfText_.getLocalBounds();
}

void Button::updateAppearance() {
    switch (state_) {
        case ButtonState::Normal:
            background_.setFillColor(normalColor_);
            sfText_.setFillColor(textColor_);
            break;
        case ButtonState::Hovered:
            background_.setFillColor(hoverColor_);
            sfText_.setFillColor(textColor_);
            break;
        case ButtonState::Pressed:
            background_.setFillColor(pressedColor_);
            sfText_.setFillColor(textColor_);
            break;
        case ButtonState::Disabled:
            background_.setFillColor(disabledColor_);
            sfText_.setFillColor(disabledTextColor_);
            break;
    }
    background_.setSize(getSize());
}

void Button::centerText() {
    sf::Vector2f size = getSize();
    sf::FloatRect textBounds = sfText_.getLocalBounds();
    sfText_.setOrigin(textBounds.left + textBounds.width / 2.f,
                      textBounds.top + textBounds.height / 2.f);
    sfText_.setPosition(size.x / 2.f, size.y / 2.f);
}

} // namespace engine::ui
