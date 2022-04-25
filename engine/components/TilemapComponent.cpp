#include "TilemapComponent.h"
#include <algorithm>
#include <stdexcept>

namespace engine {
namespace components {

TilemapComponent::TilemapComponent()
    : mapSize(0, 0)
    , tileSize(16, 16)
    , tilesetColumns(0)
    , defaultTint(sf::Color::White)
    , visible(true)
{
}

void TilemapComponent::Create(uint32_t width, uint32_t height, uint32_t tileW, uint32_t tileH)
{
    mapSize = sf::Vector2u(width, height);
    tileSize = sf::Vector2u(tileW, tileH);
    layers.clear();
    AddLayer("Default", 0);
}

void TilemapComponent::AddLayer(const std::string& name, int zOrder)
{
    TilemapLayer layer;
    layer.name = name;
    layer.zOrder = zOrder;
    layer.tiles.resize(mapSize.x * mapSize.y);
    layers.push_back(layer);

    std::sort(layers.begin(), layers.end(), [](const TilemapLayer& a, const TilemapLayer& b) {
        return a.zOrder < b.zOrder;
    });
}

void TilemapComponent::SetTile(int layer, uint32_t x, uint32_t y, uint32_t tileId)
{
    if (layer < 0 || layer >= static_cast<int>(layers.size())) return;
    if (!IsInBounds(x, y)) return;

    size_t index = static_cast<size_t>(y) * mapSize.x + x;
    layers[layer].tiles[index].tileId = tileId;
}

void TilemapComponent::SetTileFlags(int layer, uint32_t x, uint32_t y, uint32_t flags)
{
    if (layer < 0 || layer >= static_cast<int>(layers.size())) return;
    if (!IsInBounds(x, y)) return;

    size_t index = static_cast<size_t>(y) * mapSize.x + x;
    layers[layer].tiles[index].flags = flags;
}

TileData TilemapComponent::GetTile(int layer, uint32_t x, uint32_t y) const
{
    if (layer < 0 || layer >= static_cast<int>(layers.size())) return TileData();
    if (!IsInBounds(x, y)) return TileData();

    size_t index = static_cast<size_t>(y) * mapSize.x + x;
    return layers[layer].tiles[index];
}

void TilemapComponent::SetLayerOpacity(int layer, float opacity)
{
    if (layer >= 0 && layer < static_cast<int>(layers.size())) {
        layers[layer].opacity = std::clamp(opacity, 0.0f, 1.0f);
    }
}

void TilemapComponent::SetLayerVisible(int layer, bool visible)
{
    if (layer >= 0 && layer < static_cast<int>(layers.size())) {
        layers[layer].visible = visible;
    }
}

void TilemapComponent::SetLayerTint(int layer, const sf::Color& tint)
{
    if (layer >= 0 && layer < static_cast<int>(layers.size())) {
        layers[layer].tint = tint;
    }
}

int TilemapComponent::GetLayerCount() const
{
    return static_cast<int>(layers.size());
}

TilemapLayer* TilemapComponent::GetLayer(int index)
{
    if (index >= 0 && index < static_cast<int>(layers.size())) {
        return &layers[index];
    }
    return nullptr;
}

void TilemapComponent::Clear()
{
    layers.clear();
    mapSize = sf::Vector2u(0, 0);
}

void TilemapComponent::ClearLayer(int layer)
{
    if (layer >= 0 && layer < static_cast<int>(layers.size())) {
        std::fill(layers[layer].tiles.begin(), layers[layer].tiles.end(), TileData());
    }
}

void TilemapComponent::RemoveLayer(int index)
{
    if (index >= 0 && index < static_cast<int>(layers.size())) {
        layers.erase(layers.begin() + index);
    }
}

void TilemapComponent::SetTileset(const std::string& path, uint32_t columns, sf::Vector2u gridSize)
{
    tilesetPath = path;
    tilesetColumns = columns;
    tilesetGridSize = gridSize;
}

sf::IntRect TilemapComponent::GetTileTextureRect(uint32_t tileId) const
{
    if (tileId == TileData::Empty || tilesetColumns == 0) {
        return sf::IntRect(0, 0, 0, 0);
    }

    uint32_t tx = tileId % tilesetColumns;
    uint32_t ty = tileId / tilesetColumns;
    return sf::IntRect(
        static_cast<int>(tx * tileSize.x),
        static_cast<int>(ty * tileSize.y),
        static_cast<int>(tileSize.x),
        static_cast<int>(tileSize.y)
    );
}

uint32_t TilemapComponent::GetTotalTileCount() const
{
    uint32_t count = 0;
    for (const auto& layer : layers) {
        count += static_cast<uint32_t>(layer.tiles.size());
    }
    return count;
}

bool TilemapComponent::IsInBounds(uint32_t x, uint32_t y) const
{
    return x < mapSize.x && y < mapSize.y;
}

} // namespace components
} // namespace engine
