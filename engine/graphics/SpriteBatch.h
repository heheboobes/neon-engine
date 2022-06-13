#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <cstdint>

namespace engine {
namespace graphics {

struct AtlasRegion {
    sf::IntRect bounds;
    sf::Vector2f anchor;
    float rotation;
    std::string name;

    AtlasRegion() : rotation(0.f) {}
};

struct BatchedSprite {
    sf::Vector2f position;
    sf::Vector2f size;
    float rotation;
    sf::Color color;
    sf::Vector2f origin;
    int textureId;
    std::size_t regionIndex;
    sf::BlendMode blendMode;
    float depth;

    BatchedSprite()
        : rotation(0.f)
        , color(sf::Color::White)
        , origin(0.f, 0.f)
        , textureId(-1)
        , regionIndex(0)
        , blendMode(sf::BlendAlpha)
        , depth(0.f) {}
};

struct SpriteBatchGlyph {
    sf::Vertex vertices[4];
    int textureId;
    sf::BlendMode blendMode;
    float depth;
};

class SpriteBatch {
public:
    static constexpr std::size_t DEFAULT_BATCH_SIZE = 2048;
    static constexpr std::size_t MAX_VERTICES = DEFAULT_BATCH_SIZE * 4;
    static constexpr std::size_t MAX_INDICES = DEFAULT_BATCH_SIZE * 6;

    SpriteBatch();
    explicit SpriteBatch(std::size_t reserveSize);
    ~SpriteBatch();

    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch& operator=(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&& other) noexcept;
    SpriteBatch& operator=(SpriteBatch&& other) noexcept;

    void begin(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default);
    void end();

    void draw(const BatchedSprite& sprite);
    void draw(const sf::Sprite& sprite, int textureId = -1, float depth = 0.f);
    void drawQuad(const sf::Vector2f& topLeft, const sf::Vector2f& topRight,
                  const sf::Vector2f& bottomLeft, const sf::Vector2f& bottomRight,
                  const sf::Color& color, int textureId = -1,
                  const sf::Vector2f& texTopLeft = {0.f, 0.f},
                  const sf::Vector2f& texBottomRight = {1.f, 1.f});

    void flush();

    void setTexture(int id, const sf::Texture& texture);
    void removeTexture(int id);
    bool hasTexture(int id) const;
    const sf::Texture* getTexture(int id) const;

    int addAtlasRegion(int textureId, const AtlasRegion& region);
    const AtlasRegion* getAtlasRegion(int textureId, std::size_t index) const;

    void setDefaultBlendMode(sf::BlendMode mode);
    sf::BlendMode getDefaultBlendMode() const;

    void sortByTexture();
    void sortByDepth();
    void sortByTextureThenDepth();

    std::size_t getBatchCount() const;
    std::size_t getSpriteCount() const;
    void clear();

private:
    void insertGlyph(const SpriteBatchGlyph& glyph);
    void setupVertexArray();
    void buildIndexBuffer(std::size_t spriteCount);

    sf::RenderTarget* m_target = nullptr;
    sf::RenderStates m_baseStates;

    std::vector<BatchedSprite> m_sprites;
    std::vector<SpriteBatchGlyph> m_glyphs;
    std::vector<sf::Vertex> m_vertexBuffer;
    std::vector<std::uint32_t> m_indexBuffer;

    std::unordered_map<int, sf::Texture> m_textures;
    std::unordered_map<int, std::vector<AtlasRegion>> m_atlases;

    sf::BlendMode m_defaultBlend;
    bool m_batching;
    bool m_sorted;

    std::size_t m_reservedSize;
};

} // namespace graphics
} // namespace engine
