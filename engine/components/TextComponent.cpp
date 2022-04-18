#include "TextComponent.h"
#include <algorithm>

namespace engine {
namespace components {

TextComponent::TextComponent()
    : font(nullptr)
    , fontSize(24)
    , textColor(sf::Color::White)
    , lineSpacing(1.0f)
    , letterSpacing(1.0f)
    , alignment(TextAlignment::Left)
    , verticalAlignment(TextVerticalAlignment::Top)
    , visible(true)
    , autoSize(true)
{
    text.setFillColor(textColor);
    text.setCharacterSize(fontSize);
}

void TextComponent::SetFont(const std::string& filepath)
{
    auto f = std::make_shared<sf::Font>();
    if (f->loadFromFile(filepath)) {
        font = std::move(f);
        fontPath = filepath;
        text.setFont(*font);
    }
}

void TextComponent::SetFont(std::shared_ptr<sf::Font> fontPtr)
{
    if (fontPtr) {
        font = std::move(fontPtr);
        text.setFont(*font);
    }
}

void TextComponent::SetContent(const std::string& textContent)
{
    content = textContent;
    text.setString(content);
    if (autoSize) {
        ApplyAlignment();
    }
}

void TextComponent::SetFontSize(unsigned int size)
{
    fontSize = std::max(1u, size);
    text.setCharacterSize(fontSize);
}

void TextComponent::SetColor(const sf::Color& color)
{
    textColor = color;
    text.setFillColor(textColor);
}

void TextComponent::SetAlignment(TextAlignment align)
{
    alignment = align;
    ApplyAlignment();
}

void TextComponent::SetVerticalAlignment(TextVerticalAlignment align)
{
    verticalAlignment = align;
    ApplyAlignment();
}

void TextComponent::SetLineSpacing(float spacing)
{
    lineSpacing = std::max(0.5f, spacing);
    text.setLineSpacing(lineSpacing);
}

void TextComponent::SetLetterSpacing(float spacing)
{
    letterSpacing = std::max(0.0f, spacing);
    text.setLetterSpacing(letterSpacing);
}

std::string TextComponent::GetContent() const
{
    return content;
}

sf::FloatRect TextComponent::GetLocalBounds() const
{
    return text.getLocalBounds();
}

sf::FloatRect TextComponent::GetGlobalBounds() const
{
    return text.getGlobalBounds();
}

sf::Vector2f TextComponent::GetSize() const
{
    sf::FloatRect bounds = text.getLocalBounds();
    return sf::Vector2f(bounds.width, bounds.height);
}

void TextComponent::SetStyle(sf::Text::Style style)
{
    text.setStyle(style);
}

void TextComponent::AddStyle(sf::Text::Style style)
{
    text.setStyle(text.getStyle() | style);
}

void TextComponent::RemoveStyle(sf::Text::Style style)
{
    text.setStyle(text.getStyle() & ~style);
}

void TextComponent::SetOutlineColor(const sf::Color& color)
{
    text.setOutlineColor(color);
}

void TextComponent::SetOutlineThickness(float thickness)
{
    text.setOutlineThickness(thickness);
}

void TextComponent::ApplyAlignment()
{
    sf::FloatRect bounds = text.getLocalBounds();

    switch (alignment) {
        case TextAlignment::Left:
            text.setOrigin(0.f, text.getOrigin().y);
            break;
        case TextAlignment::Center:
            text.setOrigin(bounds.width / 2.f, text.getOrigin().y);
            break;
        case TextAlignment::Right:
            text.setOrigin(bounds.width, text.getOrigin().y);
            break;
    }

    switch (verticalAlignment) {
        case TextVerticalAlignment::Top:
            text.setOrigin(text.getOrigin().x, 0.f);
            break;
        case TextVerticalAlignment::Middle:
            text.setOrigin(text.getOrigin().x, bounds.height / 2.f);
            break;
        case TextVerticalAlignment::Bottom:
            text.setOrigin(text.getOrigin().x, bounds.height);
            break;
    }
}

void TextComponent::Refresh()
{
    text.setString(content);
    ApplyAlignment();
}

} // namespace components
} // namespace engine
