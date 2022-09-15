#include "Tilemap.h"
#include <cmath>
#include <cstring>

namespace engine::tilemap {

Chunk::Chunk()
    : gridX(0)
    , gridY(0)
    , vertices(sf::Quads)
    , dirty(true)
{
}

Tilemap::Tilemap()
    : m_width(0)
    , m_height(0)
    , m_tileWidth(16)
    , m_tileHeight(16)
    , m_chunkSize(16)
    , m_animated(false)
    , m_animTimer(0.f)
{
}

Tilemap::~Tilemap() = default;

void Tilemap::create(int width, int height, int tileWidth, int tileHeight) {
    m_width = width;
    m_height = height;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_layers.clear();
    m_chunks.clear();
}

void Tilemap::setTileSize(int width, int height) { m_tileWidth = width; m_tileHeight = height; }
sf::Vector2i Tilemap::getTileSize() const { return {m_tileWidth, m_tileHeight}; }
sf::Vector2i Tilemap::getMapSize() const { return {m_width, m_height}; }

int Tilemap::addLayer(std::unique_ptr<TileLayer> layer) {
    int index = static_cast<int>(m_layers.size());
    m_layers.push_back(std::move(layer));
    rebuildAll();
    return index;
}

TileLayer* Tilemap::getLayer(int index) {
    if (index < 0 || index >= static_cast<int>(m_layers.size())) return nullptr;
    return m_layers[index].get();
}

const TileLayer* Tilemap::getLayer(int index) const {
    if (index < 0 || index >= static_cast<int>(m_layers.size())) return nullptr;
    return m_layers[index].get();
}

int Tilemap::getLayerCount() const { return static_cast<int>(m_layers.size()); }

void Tilemap::removeLayer(int index) {
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers.erase(m_layers.begin() + index);
        rebuildAll();
    }
}

void Tilemap::swapLayers(int indexA, int indexB) {
    if (indexA >= 0 && indexA < static_cast<int>(m_layers.size()) &&
        indexB >= 0 && indexB < static_cast<int>(m_layers.size())) {
        std::swap(m_layers[indexA], m_layers[indexB]);
        rebuildAll();
    }
}

int Tilemap::addTileSet(std::unique_ptr<TileSet> tileSet) {
    int index = static_cast<int>(m_tileSets.size());

    int firstGID = 1;
    if (!m_tileSets.empty()) {
        auto& last = m_tileSets.back();
        firstGID = last->getFirstGID() + last->getTileCount();
    }
    tileSet->setFirstGID(firstGID);

    m_tileSets.push_back(std::move(tileSet));
    return index;
}

TileSet* Tilemap::getTileSet(int index) {
    if (index < 0 || index >= static_cast<int>(m_tileSets.size())) return nullptr;
    return m_tileSets[index].get();
}

const TileSet* Tilemap::getTileSet(int index) const {
    if (index < 0 || index >= static_cast<int>(m_tileSets.size())) return nullptr;
    return m_tileSets[index].get();
}

int Tilemap::getTileSetCount() const { return static_cast<int>(m_tileSets.size()); }

void Tilemap::setTile(int layer, int x, int y, int tileIndex, bool animated) {
    auto* l = getLayer(layer);
    if (!l) return;
    l->setTile(x, y, tileIndex);

    if (animated && tileIndex >= 0) {
        int cx, cy, lx, ly;
        getChunkCoords(x, y, cx, cy, lx, ly);
        auto* chunk = getOrCreateChunk(cx, cy);
        if (chunk) chunk->dirty = true;
    }
}

int Tilemap::getTile(int layer, int x, int y) const {
    auto* l = getLayer(layer);
    if (!l) return -1;
    return l->getTile(x, y);
}

void Tilemap::setChunkSize(int size) { m_chunkSize = std::max(1, size); }
int Tilemap::getChunkSize() const { return m_chunkSize; }

void Tilemap::getChunkCoords(int x, int y, int& cx, int& cy, int& lx, int& ly) const {
    cx = x / m_chunkSize;
    cy = y / m_chunkSize;
    lx = x % m_chunkSize;
    ly = y % m_chunkSize;
}

Chunk* Tilemap::getOrCreateChunk(int cx, int cy) {
    for (auto& c : m_chunks) {
        if (c.gridX == cx && c.gridY == cy) return &c;
    }
    m_chunks.push_back(Chunk());
    auto& chunk = m_chunks.back();
    chunk.gridX = cx;
    chunk.gridY = cy;
    chunk.dirty = true;
    return &chunk;
}

void Tilemap::rebuildAll() {
    for (auto& chunk : m_chunks) {
        chunk.dirty = true;
    }
}

void Tilemap::rebuildChunk(int chunkX, int chunkY) {
    for (auto& chunk : m_chunks) {
        if (chunk.gridX == chunkX && chunk.gridY == chunkY) {
            chunk.dirty = true;
            break;
        }
    }
}

void Tilemap::setAnimated(bool enabled) { m_animated = enabled; }
bool Tilemap::isAnimated() const { return m_animated; }

void Tilemap::updateAnimatedTiles(float dt) {
    if (!m_animated) return;
    m_animTimer += dt;

    for (auto& chunk : m_chunks) {
        chunk.dirty = true;
    }
}

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    auto& self = const_cast<Tilemap&>(*this);

    for (int layerIdx = 0; layerIdx < static_cast<int>(m_layers.size()); ++layerIdx) {
        auto* layer = m_layers[layerIdx].get();
        if (!layer || !layer->isVisible()) continue;

        states.blendMode = layer->getBlendMode();

        const sf::Texture* layerTex = nullptr;
        int firstGID = 1;

        if (!m_tileSets.empty()) {
            for (int t = static_cast<int>(m_tileSets.size()) - 1; t >= 0; --t) {
                auto* ts = m_tileSets[t].get();
                if (ts && ts->getTexture()) {
                    layerTex = ts->getTexture();
                    firstGID = ts->getFirstGID();
                    break;
                }
            }
        }

        if (!layerTex) continue;
        states.texture = layerTex;

        int chunkW = m_chunkSize;
        int chunkH = m_chunkSize;
        int chunksX = (m_width + chunkW - 1) / chunkW;
        int chunksY = (m_height + chunkH - 1) / chunkH;
        sf::Vector2f parallax = layer->getParallaxFactor();

        sf::RenderStates layerStates = states;
        layerStates.transform.translate(
            -self.getPosition().x * (1.f - parallax.x),
            -self.getPosition().y * (1.f - parallax.y));

        for (int cy = 0; cy < chunksY; ++cy) {
            for (int cx = 0; cx < chunksX; ++cx) {
                Chunk* chunk = nullptr;
                for (auto& c : self.m_chunks) {
                    if (c.gridX == cx && c.gridY == cy) {
                        chunk = &c;
                        break;
                    }
                }

                if (!chunk) {
                    chunk = &self.m_chunks.emplace_back();
                    chunk->gridX = cx;
                    chunk->gridY = cy;
                    chunk->dirty = true;
                }

                if (chunk->dirty) {
                    int startX = cx * chunkW;
                    int startY = cy * chunkH;
                    int endX = std::min(startX + chunkW, m_width);
                    int endY = std::min(startY + chunkH, m_height);
                    int visW = endX - startX;
                    int visH = endY - startY;

                    chunk->vertices.resize(static_cast<std::size_t>(visW * visH * 4));

                    for (int y = startY; y < endY; ++y) {
                        for (int x = startX; x < endX; ++x) {
                            int tileIndex = layer->getTile(x, y);
                            int quadIdx = ((y - startY) * visW + (x - startX)) * 4;

                            if (tileIndex < 0) {
                                for (int v = 0; v < 4; ++v) {
                                    chunk->vertices[quadIdx + v].position = sf::Vector2f(0, 0);
                                    chunk->vertices[quadIdx + v].texCoords = sf::Vector2f(0, 0);
                                    chunk->vertices[quadIdx + v].color = sf::Color::Transparent;
                                }
                                continue;
                            }

                            int localTile = tileIndex - firstGID;
                            if (localTile < 0) {
                                for (int v = 0; v < 4; ++v) {
                                    chunk->vertices[quadIdx + v].position = sf::Vector2f(0, 0);
                                    chunk->vertices[quadIdx + v].texCoords = sf::Vector2f(0, 0);
                                    chunk->vertices[quadIdx + v].color = sf::Color::Transparent;
                                }
                                continue;
                            }

                            if (m_animated && !m_tileSets.empty()) {
                                auto* ts = m_tileSets[0].get();
                                if (ts && ts->hasAnimation(localTile)) {
                                    auto frame = ts->getAnimatedTile(localTile, self.m_animTimer);
                                    localTile = frame.tileIndex;
                                }
                            }

                            TileSet* ts = nullptr;
                            for (int t = static_cast<int>(m_tileSets.size()) - 1; t >= 0; --t) {
                                if (m_tileSets[t] && localTile < m_tileSets[t]->getTileCount()) {
                                    ts = m_tileSets[t].get();
                                    break;
                                }
                            }

                            sf::IntRect texRect;
                            if (ts) {
                                texRect = ts->getTileTextureRect(localTile);
                            } else {
                                continue;
                            }

                            float px = static_cast<float>(x * m_tileWidth);
                            float py = static_cast<float>(y * m_tileHeight);
                            float tw = static_cast<float>(m_tileWidth);
                            float th = static_cast<float>(m_tileHeight);

                            sf::Vertex* q = &chunk->vertices[quadIdx];
                            q[0].position = sf::Vector2f(px, py);
                            q[1].position = sf::Vector2f(px + tw, py);
                            q[2].position = sf::Vector2f(px + tw, py + th);
                            q[3].position = sf::Vector2f(px, py + th);

                            float opacity = layer->getOpacity();
                            sf::Uint8 alpha = static_cast<sf::Uint8>(opacity * 255.f);

                            q[0].texCoords = sf::Vector2f(
                                static_cast<float>(texRect.left),
                                static_cast<float>(texRect.top));
                            q[1].texCoords = sf::Vector2f(
                                static_cast<float>(texRect.left + texRect.width),
                                static_cast<float>(texRect.top));
                            q[2].texCoords = sf::Vector2f(
                                static_cast<float>(texRect.left + texRect.width),
                                static_cast<float>(texRect.top + texRect.height));
                            q[3].texCoords = sf::Vector2f(
                                static_cast<float>(texRect.left),
                                static_cast<float>(texRect.top + texRect.height));

                            q[0].color = sf::Color(255, 255, 255, alpha);
                            q[1].color = sf::Color(255, 255, 255, alpha);
                            q[2].color = sf::Color(255, 255, 255, alpha);
                            q[3].color = sf::Color(255, 255, 255, alpha);
                        }
                    }

                    chunk->dirty = false;
                }

                target.draw(chunk->vertices, layerStates);
            }
        }
    }
}

} // namespace engine::tilemap
