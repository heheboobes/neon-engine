#include "Slider.h"
#include <algorithm>
#include <cmath>

namespace engine::ui {

Slider::Slider()
    : Widget()
{
    setSize({200.f, 20.f});
    track_.setSize({180.f, 6.f});
    track_.setFillColor(trackColor_);
    fill_.setFillColor(fillColor_);
    thumb_.setSize({thumbSize_, thumbSize_});
    thumb_.setFillColor(thumbColor_);
    updateThumbPosition();
}

Slider::Slider(SliderOrientation orientation)
    : Widget()
    , orientation_(orientation)
{
    if (orientation_ == SliderOrientation::Horizontal) {
        setSize({200.f, 20.f});
        track_.setSize({180.f, 6.f});
    } else {
        setSize({20.f, 200.f});
        track_.setSize({6.f, 180.f});
    }
    track_.setFillColor(trackColor_);
    fill_.setFillColor(fillColor_);
    thumb_.setSize({thumbSize_, thumbSize_});
    thumb_.setFillColor(thumbColor_);
    updateThumbPosition();
}

void Slider::update(float dt) {
    Widget::update(dt);
}

bool Slider::handleEvent(const sf::Event& event) {
    if (!isEnabled() || getVisibility() != Visibility::Visible) return false;

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        sf::FloatRect thumbBounds = thumb_.getGlobalBounds();
        sf::Vector2f localMouse = mouse - getPosition();
        if (thumbBounds.contains(localMouse)) {
            dragging_ = true;
            return true;
        }
        if (getBounds().contains(mouse)) {
            setValue(valueFromMouse(mouse));
            dragging_ = true;
            return true;
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (dragging_) {
            dragging_ = false;
            return true;
        }
    }

    if (event.type == sf::Event::MouseMoved && dragging_) {
        sf::Vector2f mouse(static_cast<float>(event.mouseMove.x),
                           static_cast<float>(event.mouseMove.y));
        setValue(valueFromMouse(mouse));
        return true;
    }

    return false;
}

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (getVisibility() != Visibility::Visible) return;
    states.transform.translate(getPosition());

    target.draw(track_, states);
    target.draw(fill_, states);
    target.draw(thumb_, states);

    Widget::draw(target, states);
}

void Slider::setMin(float min) {
    min_ = min;
    value_ = std::clamp(value_, min_, max_);
    updateThumbPosition();
}

float Slider::getMin() const {
    return min_;
}

void Slider::setMax(float max) {
    max_ = max;
    value_ = std::clamp(value_, min_, max_);
    updateThumbPosition();
}

float Slider::getMax() const {
    return max_;
}

void Slider::setValue(float value) {
    value_ = std::clamp(value, min_, max_);
    snapToStep();
    updateThumbPosition();
    if (onValueChange_) onValueChange_(value_);
    if (onChange) onChange(*this);
}

float Slider::getValue() const {
    return value_;
}

void Slider::setStep(float step) {
    step_ = std::max(0.01f, step);
    snapToStep();
    updateThumbPosition();
}

float Slider::getStep() const {
    return step_;
}

void Slider::setOrientation(SliderOrientation orientation) {
    orientation_ = orientation;
    if (orientation_ == SliderOrientation::Horizontal) {
        setSize({200.f, 20.f});
        track_.setSize({180.f, 6.f});
    } else {
        setSize({20.f, 200.f});
        track_.setSize({6.f, 180.f});
    }
    updateThumbPosition();
}

SliderOrientation Slider::getOrientation() const {
    return orientation_;
}

void Slider::setTrackColor(sf::Color color) {
    trackColor_ = color;
    track_.setFillColor(trackColor_);
}

void Slider::setFillColor(sf::Color color) {
    fillColor_ = color;
    fill_.setFillColor(fillColor_);
}

void Slider::setThumbColor(sf::Color color) {
    thumbColor_ = color;
    thumb_.setFillColor(thumbColor_);
}

void Slider::setThumbSize(float size) {
    thumbSize_ = size;
    thumb_.setSize({thumbSize_, thumbSize_});
    updateThumbPosition();
}

float Slider::getThumbSize() const {
    return thumbSize_;
}

void Slider::setOnValueChange(ValueCallback callback) {
    onValueChange_ = std::move(callback);
}

void Slider::updateThumbPosition() {
    float t = (max_ == min_) ? 0.f : (value_ - min_) / (max_ - min_);
    sf::Vector2f size = getSize();

    if (orientation_ == SliderOrientation::Horizontal) {
        float trackWidth = size.x - 20.f;
        float trackX = 10.f;
        float trackY = (size.y - track_.getSize().y) / 2.f;
        track_.setPosition(trackX, trackY);

        float fillWidth = trackWidth * t;
        fill_.setSize({fillWidth, track_.getSize().y});
        fill_.setPosition(trackX, trackY);

        float thumbX = trackX + trackWidth * t - thumbSize_ / 2.f;
        float thumbY = (size.y - thumbSize_) / 2.f;
        thumb_.setPosition(thumbX, thumbY);
    } else {
        float trackHeight = size.y - 20.f;
        float trackX = (size.x - track_.getSize().x) / 2.f;
        float trackY = 10.f;
        track_.setPosition(trackX, trackY);

        float fillHeight = trackHeight * t;
        fill_.setSize({track_.getSize().x, fillHeight});
        fill_.setPosition(trackX, trackY + trackHeight - fillHeight);

        float thumbX = (size.x - thumbSize_) / 2.f;
        float thumbY = trackY + trackHeight * t - thumbSize_ / 2.f;
        thumb_.setPosition(thumbX, thumbY);
    }
}

float Slider::valueFromMouse(sf::Vector2f mousePos) const {
    sf::Vector2f localPos = mousePos - getPosition();
    sf::Vector2f size = getSize();
    float t;

    if (orientation_ == SliderOrientation::Horizontal) {
        float trackWidth = size.x - 20.f;
        t = (localPos.x - 10.f) / trackWidth;
    } else {
        float trackHeight = size.y - 20.f;
        t = 1.f - (localPos.y - 10.f) / trackHeight;
    }

    t = std::clamp(t, 0.f, 1.f);
    return min_ + t * (max_ - min_);
}

void Slider::snapToStep() {
    value_ = std::round(value_ / step_) * step_;
    value_ = std::clamp(value_, min_, max_);
}

} // namespace engine::ui
