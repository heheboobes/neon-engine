#pragma once

#include "Widget.h"

namespace engine::ui {

class TextBox : public Widget {
public:
    using ChangeCallback = std::function<void(const std::string&)>;

    TextBox();
    explicit TextBox(const std::string& text);
    ~TextBox() override = default;

    void update(float dt) override;
    bool handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setFont(const sf::Font& font);
    void setFontSize(unsigned int size);
    unsigned int getFontSize() const;

    void setTextColor(sf::Color color);
    void setBackgroundColor(sf::Color color);
    void setCursorColor(sf::Color color);
    void setSelectionColor(sf::Color color);

    void setMaxLength(size_t maxLength);
    size_t getMaxLength() const;

    void setPasswordMode(bool password);
    bool isPasswordMode() const;

    void setPlaceholder(const std::string& text);
    const std::string& getPlaceholder() const;

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void setOnChange(ChangeCallback callback);

    void setCursorPosition(size_t pos);
    size_t getCursorPosition() const;

    void selectAll();
    void clearSelection();

private:
    std::string text_;
    std::string placeholder_;
    sf::Text sfText_;
    sf::Text placeholderText_;
    sf::RectangleShape background_;
    sf::RectangleShape cursor_;
    sf::RectangleShape selection_;

    sf::Color textColor_{220, 220, 220, 255};
    sf::Color backgroundColor_{35, 35, 38, 255};
    sf::Color cursorColor_{180, 180, 180, 255};
    sf::Color selectionColor_{60, 100, 180, 120};
    sf::Color placeholderColor_{120, 120, 120, 255};

    size_t cursorPos_ = 0;
    size_t selectionStart_ = 0;
    size_t selectionEnd_ = 0;
    size_t maxLength_ = 256;
    unsigned int fontSize_ = 14u;
    bool passwordMode_ = false;
    bool readOnly_ = false;
    bool focused_ = false;
    float cursorTimer_ = 0.f;
    bool cursorVisible_ = true;

    ChangeCallback onChange_;

    void insertChar(char c);
    void deleteChar();
    void updateDisplayString();
    void updateCursorPosition();
    void clampCursor();
};

} // namespace engine::ui
