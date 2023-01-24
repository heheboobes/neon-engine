#include "CheckBox.h"

namespace engine::ui {

CheckBox::CheckBox()
    : Widget()
{
    setSize({120.f, 20.f});
    box_.setSize({boxSize_, boxSize_});
    box_.setFillColor(boxColor_);
    checkMark_.setSize({boxSize_ - 6.f, boxSize_ - 6.f});
    checkMark_.setFillColor(checkColor_);
    indeterminateMark_.setSize({boxSize_ - 6.f, 3.f});
    indeterminateMark_.setFillColor(checkColor_);
    sfLabel_.setCharacterSize(fontSize_);
    sfLabel_.setFillColor(labelColor_);
}

CheckBox::CheckBox(const std::string& label)
    : Widget()
    , label_(label)
{
    setSize({120.f, 20.f});
    box_.setSize({boxSize_, boxSize_});
    box_.setFillColor(boxColor_);
    checkMark_.setSize({boxSize_ - 6.f, boxSize_ - 6.f});
    checkMark_.setFillColor(checkColor_);
    indeterminateMark_.setSize({boxSize_ - 6.f, 3.f});
    indeterminateMark_.setFillColor(checkColor_);
    sfLabel_.setCharacterSize(fontSize_);
    sfLabel_.setFillColor(labelColor_);
    sfLabel_.setString(label_);
    updateVisuals();
}

void CheckBox::update(float dt) {
    Widget::update(dt);
}

bool CheckBox::handleEvent(const sf::Event& event) {
    if (!isEnabled() || getVisibility() != Visibility::Visible) return false;

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mouse(static_cast<float>(event.mouseMove.x),
                           static_cast<float>(event.mouseMove.y));
        hovered_ = contains(mouse);
        updateVisuals();
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        if (contains(mouse) && event.mouseButton.button == sf::Mouse::Left) {
            switch (state_) {
                case CheckState::Unchecked:
                    setState(CheckState::Checked);
                    break;
                case CheckState::Checked:
                    setState(CheckState::Unchecked);
                    break;
                case CheckState::Indeterminate:
                    setState(CheckState::Checked);
                    break;
            }
            if (onToggle_) onToggle_(state_);
            if (onClick) onClick(*this);
            return true;
        }
    }

    return false;
}

void CheckBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());

    target.draw(box_, states);

    if (state_ == CheckState::Checked) {
        target.draw(checkMark_, states);
    } else if (state_ == CheckState::Indeterminate) {
        target.draw(indeterminateMark_, states);
    }

    target.draw(sfLabel_, states);
    Widget::draw(target, states);
}

void CheckBox::setLabel(const std::string& label) {
    label_ = label;
    sfLabel_.setString(label_);
    updateVisuals();
}

const std::string& CheckBox::getLabel() const {
    return label_;
}

void CheckBox::setChecked(bool checked) {
    state_ = checked ? CheckState::Checked : CheckState::Unchecked;
    updateVisuals();
}

bool CheckBox::isChecked() const {
    return state_ == CheckState::Checked;
}

void CheckBox::setState(CheckState state) {
    state_ = state;
    updateVisuals();
}

CheckState CheckBox::getState() const {
    return state_;
}

void CheckBox::setFont(const sf::Font& font) {
    sfLabel_.setFont(font);
}

void CheckBox::setFontSize(unsigned int size) {
    fontSize_ = size;
    sfLabel_.setCharacterSize(fontSize_);
}

unsigned int CheckBox::getFontSize() const {
    return fontSize_;
}

void CheckBox::setLabelColor(sf::Color color) {
    labelColor_ = color;
    sfLabel_.setFillColor(labelColor_);
}

void CheckBox::setBoxColor(sf::Color color) {
    boxColor_ = color;
    updateVisuals();
}

void CheckBox::setCheckColor(sf::Color color) {
    checkColor_ = color;
    checkMark_.setFillColor(checkColor_);
    indeterminateMark_.setFillColor(checkColor_);
}

void CheckBox::setHoverColor(sf::Color color) {
    hoverColor_ = color;
}

void CheckBox::setOnToggle(ToggleCallback callback) {
    onToggle_ = std::move(callback);
}

void CheckBox::updateVisuals() {
    float boxX = 0.f;
    float boxY = (getSize().y - boxSize_) / 2.f;
    box_.setPosition(boxX, boxY);
    box_.setFillColor(hovered_ ? hoverColor_ : boxColor_);

    checkMark_.setPosition(boxX + 3.f, boxY + 3.f);
    indeterminateMark_.setPosition(boxX + 3.f, boxY + (boxSize_ - 3.f) / 2.f);

    float labelX = boxSize_ + 6.f;
    float labelY = (getSize().y - sfLabel_.getLocalBounds().height) / 2.f;
    sfLabel_.setPosition(labelX, labelY);
}

} // namespace engine::ui
