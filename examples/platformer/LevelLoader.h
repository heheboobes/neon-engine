#pragma once

#include <engine/ecs/Entity.h>
#include <SFML/Graphics/Rect.hpp>
#include <string>
#include <vector>

namespace examples::platformer
{

struct TileData
{
    int id;
    sf::IntRect textureRect;
    bool isSolid;
    std::string texturePath;
};

struct LevelData
{
    int width;
    int height;
    int tileSize;
    std::vector<engine::ecs::Entity> entities;
    std::vector<TileData> tiles;
};

class LevelLoader
{
public:
    static LevelData load(const std::string& filepath);

private:
    static TileData parseTile(const std::string& json);
    static engine::ecs::Entity createTileEntity(const TileData& tile, int x, int y, int tileSize);
};

}
