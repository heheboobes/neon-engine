#pragma once

#include "Widget.h"

namespace engine::ui {

enum class ButtonState {
    Normal,
    Hovered,
    Pressed,
    Disabled
};

class Button : public Widget {
public:
    using ClickedCallback = std::function<void()>;

    Button();
    explicit Button(const std::string& text);
    ~Button() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setFont(const sf::Font& font);
    void setFontSize(unsigned int size);
    unsigned int getFontSize() const;

    void setTextColor(sf::Color color);
    void setNormalColor(sf::Color color);
    void setHoverColor(sf::Color color);
    void setPressedColor(sf::Color color);
    void setDisabledColor(sf::Color color);

    void setCornerRadius(float radius);
    float getCornerRadius() const;

    ButtonState getState() const;
    void setState(ButtonState state);

    void setOnClicked(ClickedCallback callback);

    sf::FloatRect getTextBounds() const;

private:
    std::string text_;
    sf::Text sfText_;
    sf::RectangleShape background_;
    ButtonState state_ = ButtonState::Normal;

    sf::Color normalColor_{60, 60, 60, 255};
    sf::Color hoverColor_{80, 80, 80, 255};
    sf::Color pressedColor_{40, 40, 40, 255};
    sf::Color disabledColor_{100, 100, 100, 128};
    sf::Color textColor_{220, 220, 220, 255};
    sf::Color disabledTextColor_{120, 120, 120, 255};

    float cornerRadius_ = 4.f;
    unsigned int fontSize_ = 14u;
    ClickedCallback onClicked_;

    void updateAppearance();
    void centerText();
};

} // namespace engine::ui
