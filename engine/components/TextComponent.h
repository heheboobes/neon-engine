#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"
#include <memory>
#include <string>

namespace engine {
namespace components {

enum class TextAlignment {
    Left,
    Center,
    Right
};

enum class TextVerticalAlignment {
    Top,
    Middle,
    Bottom
};

class TextComponent : public ecs::Component<TextComponent> {
public:
    sf::Text text;
    std::shared_ptr<sf::Font> font;
    std::string fontPath;

    std::string content;
    unsigned int fontSize;
    sf::Color textColor;
    float lineSpacing;
    float letterSpacing;

    TextAlignment alignment;
    TextVerticalAlignment verticalAlignment;
    bool visible;
    bool autoSize;

    TextComponent();

    void SetFont(const std::string& filepath);
    void SetFont(std::shared_ptr<sf::Font> fontPtr);
    void SetContent(const std::string& textContent);
    void SetFontSize(unsigned int size);
    void SetColor(const sf::Color& color);
    void SetAlignment(TextAlignment align);
    void SetVerticalAlignment(TextVerticalAlignment align);
    void SetLineSpacing(float spacing);
    void SetLetterSpacing(float spacing);

    std::string GetContent() const;
    sf::FloatRect GetLocalBounds() const;
    sf::FloatRect GetGlobalBounds() const;
    sf::Vector2f GetSize() const;

    void SetStyle(sf::Text::Style style);
    void AddStyle(sf::Text::Style style);
    void RemoveStyle(sf::Text::Style style);

    void SetOutlineColor(const sf::Color& color);
    void SetOutlineThickness(float thickness);

    void ApplyAlignment();
    void Refresh();
};

} // namespace components
} // namespace engine
