#pragma once

#include "Widget.h"

namespace engine::ui {

enum class TextAlignment {
    Left,
    Center,
    Right
};

class Label : public Widget {
public:
    Label();
    explicit Label(const std::string& text);
    ~Label() override = default;

    void update(float dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setText(const std::string& text);
    const std::string& getText() const;

    void setFont(const sf::Font& font);
    void setFontSize(unsigned int size);
    unsigned int getFontSize() const;

    void setColor(sf::Color color);
    sf::Color getColor() const;

    void setAlignment(TextAlignment alignment);
    TextAlignment getAlignment() const;

    void setWordWrap(bool wrap);
    bool getWordWrap() const;

    void setAutoSize(bool autoSize);
    bool getAutoSize() const;

    void setLineSpacing(float spacing);
    float getLineSpacing() const;

private:
    std::string text_;
    sf::Text sfText_;
    sf::Color color_{200, 200, 200, 255};
    TextAlignment alignment_ = TextAlignment::Left;
    bool wordWrap_ = false;
    bool autoSize_ = true;
    float lineSpacing_ = 1.2f;
    unsigned int fontSize_ = 14u;

    void rebuildText();
    void applyAlignment();
    std::vector<std::string> wrapText(const std::string& input, float maxWidth) const;
};

} // namespace engine::ui
