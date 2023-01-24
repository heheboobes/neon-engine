#pragma once

#include "Widget.h"

namespace engine::ui {

enum class CheckState {
    Unchecked,
    Checked,
    Indeterminate
};

class CheckBox : public Widget {
public:
    using ToggleCallback = std::function<void(CheckState)>;

    CheckBox();
    explicit CheckBox(const std::string& label);
    ~CheckBox() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setLabel(const std::string& label);
    const std::string& getLabel() const;

    void setChecked(bool checked);
    bool isChecked() const;

    void setState(CheckState state);
    CheckState getState() const;

    void setFont(const sf::Font& font);
    void setFontSize(unsigned int size);
    unsigned int getFontSize() const;

    void setLabelColor(sf::Color color);
    void setBoxColor(sf::Color color);
    void setCheckColor(sf::Color color);
    void setHoverColor(sf::Color color);

    void setOnToggle(ToggleCallback callback);

private:
    std::string label_;
    CheckState state_ = CheckState::Unchecked;

    sf::RectangleShape box_;
    sf::RectangleShape checkMark_;
    sf::RectangleShape indeterminateMark_;
    sf::Text sfLabel_;

    sf::Color boxColor_{60, 60, 65, 255};
    sf::Color checkColor_{80, 140, 220, 255};
    sf::Color labelColor_{220, 220, 220, 255};
    sf::Color hoverColor_{80, 80, 85, 255};
    unsigned int fontSize_ = 14u;
    float boxSize_ = 16.f;
    bool hovered_ = false;

    ToggleCallback onToggle_;

    void updateVisuals();
};

} // namespace engine::ui
