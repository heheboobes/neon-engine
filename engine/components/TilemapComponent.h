#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector2u.hpp>
#include "../../engine/ecs/Component.h"
#include <vector>
#include <cstdint>
#include <string>

namespace engine {
namespace components {

struct TileData {
    uint32_t tileId;
    uint32_t flags;
    sf::Color tint;

    static constexpr uint32_t FlipX = 1 << 0;
    static constexpr uint32_t FlipY = 1 << 1;
    static constexpr uint32_t FlipD = 1 << 2;
    static constexpr uint32_t Empty = 0xFFFFFFFF;

    TileData() : tileId(Empty), flags(0), tint(sf::Color::White) {}
    explicit TileData(uint32_t id) : tileId(id), flags(0), tint(sf::Color::White) {}
};

struct TilemapLayer {
    std::vector<TileData> tiles;
    std::string name;
    float opacity;
    bool visible;
    int zOrder;
    sf::Color tint;

    TilemapLayer() : opacity(1.0f), visible(true), zOrder(0), tint(sf::Color::White) {}
};

class TilemapComponent : public ecs::Component<TilemapComponent> {
public:
    std::vector<TilemapLayer> layers;
    sf::Vector2u mapSize;
    sf::Vector2u tileSize;

    std::string tilesetPath;
    sf::Vector2u tilesetGridSize;
    uint32_t tilesetColumns;

    sf::Color defaultTint;
    bool visible;

    TilemapComponent();

    void Create(uint32_t width, uint32_t height, uint32_t tileW, uint32_t tileH);
    void AddLayer(const std::string& name, int zOrder = 0);

    void SetTile(int layer, uint32_t x, uint32_t y, uint32_t tileId);
    void SetTileFlags(int layer, uint32_t x, uint32_t y, uint32_t flags);
    TileData GetTile(int layer, uint32_t x, uint32_t y) const;

    void SetLayerOpacity(int layer, float opacity);
    void SetLayerVisible(int layer, bool visible);
    void SetLayerTint(int layer, const sf::Color& tint);

    int GetLayerCount() const;
    TilemapLayer* GetLayer(int index);

    void Clear();
    void ClearLayer(int layer);
    void RemoveLayer(int index);

    void SetTileset(const std::string& path, uint32_t columns, sf::Vector2u gridSize);
    sf::IntRect GetTileTextureRect(uint32_t tileId) const;

    uint32_t GetTotalTileCount() const;
    bool IsInBounds(uint32_t x, uint32_t y) const;
};

} // namespace components
} // namespace engine
