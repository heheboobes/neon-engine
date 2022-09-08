#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>

namespace engine::tilemap {

struct TileAnimationFrame {
    int tileIndex;
    float duration;
};

struct TileAnimation {
    int currentFrame;
    float timer;
    bool looping;
    std::vector<TileAnimationFrame> frames;
};

struct TileCollision {
    bool solid;
    bool oneWay;
    unsigned int flags;

    TileCollision();
};

class TileSet {
public:
    TileSet();

    bool loadFromFile(const std::string& texturePath, int tileWidth, int tileHeight,
                      int spacing = 0, int margin = 0);
    bool loadFromTexture(const sf::Texture& texture, int tileWidth, int tileHeight,
                         int spacing = 0, int margin = 0);

    int getTileWidth() const;
    int getTileHeight() const;
    int getColumns() const;
    int getRows() const;
    int getTileCount() const;
    int getSpacing() const;
    int getMargin() const;

    sf::IntRect getTileTextureRect(int tileIndex) const;
    const sf::Texture* getTexture() const;

    void setCollision(int tileIndex, bool solid, unsigned int flags = 0);
    const TileCollision& getCollision(int tileIndex) const;
    bool isTileSolid(int tileIndex) const;

    void addAnimation(int tileIndex, const TileAnimation& anim);
    bool hasAnimation(int tileIndex) const;
    const TileAnimation& getAnimation(int tileIndex) const;
    TileAnimationFrame getAnimatedTile(int tileIndex, float dt);

    int getFirstGID() const;
    void setFirstGID(int gid);

private:
    const sf::Texture* m_texture;
    bool m_ownsTexture;
    int m_tileWidth;
    int m_tileHeight;
    int m_spacing;
    int m_margin;
    int m_columns;
    int m_rows;
    int m_firstGID;

    std::unordered_map<int, TileCollision> m_collisionMap;
    std::unordered_map<int, TileAnimation> m_animations;
};

} // namespace engine::tilemap
