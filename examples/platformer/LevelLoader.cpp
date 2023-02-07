#include "LevelLoader.h"
#include <engine/graphics/SpriteComponent.h>
#include <engine/physics/TransformComponent.h>
#include <engine/physics/ColliderComponent.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace examples::platformer
{

LevelData LevelLoader::load(const std::string& filepath)
{
    LevelData level;
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open level file: " << filepath << "\n";
        return level;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    size_t wPos = content.find("\"width\"");
    size_t hPos = content.find("\"height\"");
    size_t tsPos = content.find("\"tileSize\"");
    size_t dataPos = content.find("\"tiles\"");

    if (wPos != std::string::npos)
        level.width = std::stoi(content.substr(wPos + 8));
    if (hPos != std::string::npos)
        level.height = std::stoi(content.substr(hPos + 9));
    if (tsPos != std::string::npos)
        level.tileSize = std::stoi(content.substr(tsPos + 11));

    if (dataPos != std::string::npos)
    {
        size_t start = content.find('[', dataPos);
        size_t end = content.find(']', start);
        std::string tileArray = content.substr(start, end - start + 1);

        size_t pos = 0;
        int index = 0;
        while ((pos = tileArray.find('{', pos)) != std::string::npos)
        {
            size_t close = tileArray.find('}', pos);
            std::string tileStr = tileArray.substr(pos, close - pos + 1);
            TileData tile = parseTile(tileStr);
            int tx = index % level.width;
            int ty = index / level.width;
            level.entities.push_back(createTileEntity(tile, tx, ty, level.tileSize));
            level.tiles.push_back(tile);
            pos = close + 1;
            ++index;
        }
    }

    file.close();
    return level;
}

TileData LevelLoader::parseTile(const std::string& json)
{
    TileData tile{};
    size_t idPos = json.find("\"id\"");
    if (idPos != std::string::npos)
        tile.id = std::stoi(json.substr(idPos + 5));

    size_t solidPos = json.find("\"solid\"");
    if (solidPos != std::string::npos)
        tile.isSolid = (json.find("true", solidPos) != std::string::npos);

    size_t texPos = json.find("\"texture\"");
    if (texPos != std::string::npos)
    {
        size_t q1 = json.find('"', texPos + 10);
        size_t q2 = json.find('"', q1 + 1);
        tile.texturePath = json.substr(q1 + 1, q2 - q1 - 1);
    }

    tile.textureRect = sf::IntRect(0, 0, 32, 32);
    return tile;
}

engine::ecs::Entity LevelLoader::createTileEntity(const TileData& tile, int x, int y, int tileSize)
{
    engine::ecs::Entity entity;
    entity.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize)),
        0.f,
        sf::Vector2f(1.f, 1.f)
    );
    entity.addComponent<engine::graphics::SpriteComponent>(tile.texturePath);
    if (tile.isSolid)
    {
        entity.addComponent<engine::physics::ColliderComponent>(
            sf::FloatRect(0.f, 0.f, static_cast<float>(tileSize), static_cast<float>(tileSize)),
            engine::physics::ColliderType::Static
        );
    }
    return entity;
}

}
