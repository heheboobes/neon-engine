#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace engine::tilemap {

class TileLayer {
public:
    TileLayer();

    void create(int width, int height, int tileWidth, int tileHeight);
    void resize(int width, int height);

    int getWidth() const;
    int getHeight() const;
    int getTileWidth() const;
    int getTileHeight() const;
    std::size_t getTileCount() const;

    void setTile(int x, int y, int tileIndex);
    int getTile(int x, int y) const;
    bool isInBounds(int x, int y) const;

    void setTileRow(int y, const std::vector<int>& tiles);
    void fill(int tileIndex);
    void fillRect(int x, int y, int w, int h, int tileIndex);

    void setVisible(bool visible);
    bool isVisible() const;

    void setParallaxFactor(const sf::Vector2f& factor);
    const sf::Vector2f& getParallaxFactor() const;

    void setBlendMode(sf::BlendMode mode);
    sf::BlendMode getBlendMode() const;

    void setOpacity(float opacity);
    float getOpacity() const;

    void setName(const std::string& name);
    const std::string& getName() const;

    const std::vector<int>& getTiles() const;
    std::vector<int>& getTiles();

private:
    std::vector<int> m_tiles;
    int m_width;
    int m_height;
    int m_tileWidth;
    int m_tileHeight;
    bool m_visible;
    sf::Vector2f m_parallaxFactor;
    sf::BlendMode m_blendMode;
    float m_opacity;
    std::string m_name;
};

} // namespace engine::tilemap
