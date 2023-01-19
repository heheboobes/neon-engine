#pragma once

#include "Widget.h"

namespace engine::ui {

enum class SliderOrientation {
    Horizontal,
    Vertical
};

class Slider : public Widget {
public:
    using ValueCallback = std::function<void(float)>;

    Slider();
    explicit Slider(SliderOrientation orientation);
    ~Slider() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setMin(float min);
    float getMin() const;

    void setMax(float max);
    float getMax() const;

    void setValue(float value);
    float getValue() const;

    void setStep(float step);
    float getStep() const;

    void setOrientation(SliderOrientation orientation);
    SliderOrientation getOrientation() const;

    void setTrackColor(sf::Color color);
    void setFillColor(sf::Color color);
    void setThumbColor(sf::Color color);
    void setThumbSize(float size);
    float getThumbSize() const;

    void setOnValueChange(ValueCallback callback);

private:
    float min_ = 0.f;
    float max_ = 100.f;
    float value_ = 50.f;
    float step_ = 1.f;
    SliderOrientation orientation_ = SliderOrientation::Horizontal;

    sf::RectangleShape track_;
    sf::RectangleShape fill_;
    sf::RectangleShape thumb_;
    sf::Color trackColor_{60, 60, 65, 255};
    sf::Color fillColor_{80, 140, 220, 255};
    sf::Color thumbColor_{180, 180, 190, 255};
    float thumbSize_ = 14.f;
    bool dragging_ = false;

    ValueCallback onValueChange_;

    void updateThumbPosition();
    float valueFromMouse(sf::Vector2f mousePos) const;
    void snapToStep();
};

} // namespace engine::ui
