#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "TileLayer.h"
#include "TileSet.h"

namespace engine::tilemap {

struct Chunk {
    int gridX;
    int gridY;
    sf::VertexArray vertices;
    bool dirty;

    Chunk();
};

class Tilemap : public sf::Drawable, public sf::Transformable {
public:
    Tilemap();
    ~Tilemap() override;

    void create(int width, int height, int tileWidth, int tileHeight);
    void setTileSize(int width, int height);
    sf::Vector2i getTileSize() const;
    sf::Vector2i getMapSize() const;

    int addLayer(std::unique_ptr<TileLayer> layer);
    TileLayer* getLayer(int index);
    const TileLayer* getLayer(int index) const;
    int getLayerCount() const;
    void removeLayer(int index);
    void swapLayers(int indexA, int indexB);

    int addTileSet(std::unique_ptr<TileSet> tileSet);
    TileSet* getTileSet(int index);
    const TileSet* getTileSet(int index) const;
    int getTileSetCount() const;

    void setTile(int layer, int x, int y, int tileIndex, bool animated = false);
    int getTile(int layer, int x, int y) const;

    void setChunkSize(int size);
    int getChunkSize() const;

    void rebuildAll();
    void rebuildChunk(int chunkX, int chunkY);

    void updateAnimatedTiles(float dt);
    void setAnimated(bool enabled);
    bool isAnimated() const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void getChunkCoords(int x, int y, int& cx, int& cy, int& lx, int& ly) const;
    Chunk* getOrCreateChunk(int cx, int cy);

    int m_width;
    int m_height;
    int m_tileWidth;
    int m_tileHeight;
    int m_chunkSize;

    std::vector<std::unique_ptr<TileLayer>> m_layers;
    std::vector<std::unique_ptr<TileSet>> m_tileSets;
    std::vector<Chunk> m_chunks;
    bool m_animated;
    float m_animTimer;
};

} // namespace engine::tilemap
// v2: Collision auto-detection
