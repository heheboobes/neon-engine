#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../engine/ecs/Component.h"
#include <memory>
#include <string>

namespace engine {
namespace components {

class SpriteComponent : public ecs::Component<SpriteComponent> {
public:
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    std::string texturePath;

    bool visible;
    bool flipX;
    bool flipY;
    uint8_t opacity;

    SpriteComponent();
    explicit SpriteComponent(const std::string& textureFile);

    void SetTexture(const std::string& filepath);
    void SetTexture(std::shared_ptr<sf::Texture> tex);
    void SetTextureRect(const sf::IntRect& rect);
    void SetColor(const sf::Color& color);
    void SetOpacity(uint8_t alpha);

    void SetFlipX(bool flip);
    void SetFlipY(bool flip);

    sf::Vector2f GetSize() const;
    sf::FloatRect GetGlobalBounds() const;

    void SetOriginCenter();
    void SetOrigin(const sf::Vector2f& origin);

    bool HasTexture() const;
    void ReloadTexture();
};

} // namespace components
} // namespace engine
// v2: Added flip X/Y
