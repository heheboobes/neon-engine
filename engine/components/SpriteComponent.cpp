#include "SpriteComponent.h"

namespace engine {
namespace components {

SpriteComponent::SpriteComponent()
    : texture(nullptr)
    , visible(true)
    , flipX(false)
    , flipY(false)
    , opacity(255)
{
    sprite.setColor(sf::Color(255, 255, 255, opacity));
}

SpriteComponent::SpriteComponent(const std::string& textureFile)
    : texture(nullptr)
    , texturePath(textureFile)
    , visible(true)
    , flipX(false)
    , flipY(false)
    , opacity(255)
{
    if (!textureFile.empty()) {
        SetTexture(textureFile);
    }
    sprite.setColor(sf::Color(255, 255, 255, opacity));
}

void SpriteComponent::SetTexture(const std::string& filepath)
{
    auto tex = std::make_shared<sf::Texture>();
    if (tex->loadFromFile(filepath)) {
        texture = tex;
        texturePath = filepath;
        sprite.setTexture(*texture, true);
    }
}

void SpriteComponent::SetTexture(std::shared_ptr<sf::Texture> tex)
{
    if (tex) {
        texture = std::move(tex);
        sprite.setTexture(*texture, true);
    }
}

void SpriteComponent::SetTextureRect(const sf::IntRect& rect)
{
    sprite.setTextureRect(rect);
}

void SpriteComponent::SetColor(const sf::Color& color)
{
    sf::Color c = color;
    c.a = opacity;
    sprite.setColor(c);
}

void SpriteComponent::SetOpacity(uint8_t alpha)
{
    opacity = alpha;
    sf::Color c = sprite.getColor();
    c.a = alpha;
    sprite.setColor(c);
}

void SpriteComponent::SetFlipX(bool flip)
{
    flipX = flip;
    sf::IntRect rect = sprite.getTextureRect();
    if (flip) {
        rect.left = rect.left + rect.width;
        rect.width = -std::abs(rect.width);
    } else {
        rect.left = std::abs(rect.left);
        rect.width = std::abs(rect.width);
    }
    sprite.setTextureRect(rect);
}

void SpriteComponent::SetFlipY(bool flip)
{
    flipY = flip;
    sf::IntRect rect = sprite.getTextureRect();
    if (flip) {
        rect.top = rect.top + rect.height;
        rect.height = -std::abs(rect.height);
    } else {
        rect.top = std::abs(rect.top);
        rect.height = std::abs(rect.height);
    }
    sprite.setTextureRect(rect);
}

sf::Vector2f SpriteComponent::GetSize() const
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    return sf::Vector2f(bounds.width, bounds.height);
}

sf::FloatRect SpriteComponent::GetGlobalBounds() const
{
    return sprite.getGlobalBounds();
}

void SpriteComponent::SetOriginCenter()
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void SpriteComponent::SetOrigin(const sf::Vector2f& origin)
{
    sprite.setOrigin(origin);
}

bool SpriteComponent::HasTexture() const
{
    return texture != nullptr && sprite.getTexture() != nullptr;
}

void SpriteComponent::ReloadTexture()
{
    if (!texturePath.empty()) {
        SetTexture(texturePath);
    }
}

} // namespace components
} // namespace engine
