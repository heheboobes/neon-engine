#include "TileLayer.h"
#include <cstring>
#include <algorithm>

namespace engine::tilemap {

TileLayer::TileLayer()
    : m_width(0)
    , m_height(0)
    , m_tileWidth(16)
    , m_tileHeight(16)
    , m_visible(true)
    , m_parallaxFactor(1.f, 1.f)
    , m_blendMode(sf::BlendAlpha)
    , m_opacity(1.f)
{
}

void TileLayer::create(int width, int height, int tileWidth, int tileHeight) {
    m_width = width;
    m_height = height;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_tiles.resize(static_cast<std::size_t>(width * height), -1);
}

void TileLayer::resize(int width, int height) {
    if (width == m_width && height == m_height) return;

    std::vector<int> newTiles(static_cast<std::size_t>(width * height), -1);

    int copyW = std::min(width, m_width);
    int copyH = std::min(height, m_height);

    for (int y = 0; y < copyH; ++y) {
        std::memcpy(
            &newTiles[static_cast<std::size_t>(y * width)],
            &m_tiles[static_cast<std::size_t>(y * m_width)],
            static_cast<std::size_t>(copyW) * sizeof(int));
    }

    m_tiles = std::move(newTiles);
    m_width = width;
    m_height = height;
}

int TileLayer::getWidth() const { return m_width; }
int TileLayer::getHeight() const { return m_height; }
int TileLayer::getTileWidth() const { return m_tileWidth; }
int TileLayer::getTileHeight() const { return m_tileHeight; }
std::size_t TileLayer::getTileCount() const { return m_tiles.size(); }

void TileLayer::setTile(int x, int y, int tileIndex) {
    if (isInBounds(x, y)) {
        m_tiles[static_cast<std::size_t>(y * m_width + x)] = tileIndex;
    }
}

int TileLayer::getTile(int x, int y) const {
    if (isInBounds(x, y)) {
        return m_tiles[static_cast<std::size_t>(y * m_width + x)];
    }
    return -1;
}

bool TileLayer::isInBounds(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

void TileLayer::setTileRow(int y, const std::vector<int>& tiles) {
    if (y < 0 || y >= m_height) return;

    int count = std::min(static_cast<int>(tiles.size()), m_width);
    std::memcpy(
        &m_tiles[static_cast<std::size_t>(y * m_width)],
        tiles.data(),
        static_cast<std::size_t>(count) * sizeof(int));
}

void TileLayer::fill(int tileIndex) {
    std::fill(m_tiles.begin(), m_tiles.end(), tileIndex);
}

void TileLayer::fillRect(int x, int y, int w, int h, int tileIndex) {
    int xEnd = std::min(x + w, m_width);
    int yEnd = std::min(y + h, m_height);

    for (int cy = std::max(0, y); cy < yEnd; ++cy) {
        for (int cx = std::max(0, x); cx < xEnd; ++cx) {
            m_tiles[static_cast<std::size_t>(cy * m_width + cx)] = tileIndex;
        }
    }
}

void TileLayer::setVisible(bool visible) { m_visible = visible; }
bool TileLayer::isVisible() const { return m_visible; }

void TileLayer::setParallaxFactor(const sf::Vector2f& factor) { m_parallaxFactor = factor; }
const sf::Vector2f& TileLayer::getParallaxFactor() const { return m_parallaxFactor; }

void TileLayer::setBlendMode(sf::BlendMode mode) { m_blendMode = mode; }
sf::BlendMode TileLayer::getBlendMode() const { return m_blendMode; }

void TileLayer::setOpacity(float opacity) { m_opacity = std::clamp(opacity, 0.f, 1.f); }
float TileLayer::getOpacity() const { return m_opacity; }

void TileLayer::setName(const std::string& name) { m_name = name; }
const std::string& TileLayer::getName() const { return m_name; }

const std::vector<int>& TileLayer::getTiles() const { return m_tiles; }
std::vector<int>& TileLayer::getTiles() { return m_tiles; }

} // namespace engine::tilemap
