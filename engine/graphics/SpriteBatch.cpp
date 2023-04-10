#include "SpriteBatch.h"
#include <algorithm>
#include <cstring>
#include <cassert>

namespace engine {
namespace graphics {

SpriteBatch::SpriteBatch()
    : m_defaultBlend(sf::BlendAlpha)
    , m_batching(false)
    , m_sorted(false)
    , m_reservedSize(DEFAULT_BATCH_SIZE) {
    m_sprites.reserve(m_reservedSize);
    m_glyphs.reserve(m_reservedSize);
    m_vertexBuffer.reserve(MAX_VERTICES);
    m_indexBuffer.reserve(MAX_INDICES);
}

SpriteBatch::SpriteBatch(std::size_t reserveSize)
    : m_defaultBlend(sf::BlendAlpha)
    , m_batching(false)
    , m_sorted(false)
    , m_reservedSize(reserveSize) {
    m_sprites.reserve(reserveSize);
    m_glyphs.reserve(reserveSize);
    m_vertexBuffer.reserve(reserveSize * 4);
    m_indexBuffer.reserve(reserveSize * 6);
}

SpriteBatch::~SpriteBatch() {
    end();
}

SpriteBatch::SpriteBatch(SpriteBatch&& other) noexcept
    : m_target(other.m_target)
    , m_baseStates(other.m_baseStates)
    , m_sprites(std::move(other.m_sprites))
    , m_glyphs(std::move(other.m_glyphs))
    , m_vertexBuffer(std::move(other.m_vertexBuffer))
    , m_indexBuffer(std::move(other.m_indexBuffer))
    , m_textures(std::move(other.m_textures))
    , m_atlases(std::move(other.m_atlases))
    , m_defaultBlend(other.m_defaultBlend)
    , m_batching(other.m_batching)
    , m_sorted(other.m_sorted)
    , m_reservedSize(other.m_reservedSize) {
    other.m_target = nullptr;
    other.m_batching = false;
}

SpriteBatch& SpriteBatch::operator=(SpriteBatch&& other) noexcept {
    if (this != &other) {
        m_target = other.m_target;
        m_baseStates = other.m_baseStates;
        m_sprites = std::move(other.m_sprites);
        m_glyphs = std::move(other.m_glyphs);
        m_vertexBuffer = std::move(other.m_vertexBuffer);
        m_indexBuffer = std::move(other.m_indexBuffer);
        m_textures = std::move(other.m_textures);
        m_atlases = std::move(other.m_atlases);
        m_defaultBlend = other.m_defaultBlend;
        m_batching = other.m_batching;
        m_sorted = other.m_sorted;
        m_reservedSize = other.m_reservedSize;
        other.m_target = nullptr;
        other.m_batching = false;
    }
    return *this;
}

void SpriteBatch::begin(sf::RenderTarget& target, sf::RenderStates states) {
    assert(!m_batching && "SpriteBatch::begin called while already batching");
    m_target = &target;
    m_baseStates = states;
    m_batching = true;
    m_sorted = false;
}

void SpriteBatch::end() {
    if (m_batching) {
        flush();
        m_target = nullptr;
        m_batching = false;
    }
}

void SpriteBatch::draw(const BatchedSprite& sprite) {
    assert(m_batching && "SpriteBatch::draw called outside begin/end pair");
    m_sprites.push_back(sprite);
    m_sorted = false;
}

void SpriteBatch::draw(const sf::Sprite& sprite, int textureId, float depth) {
    assert(m_batching && "SpriteBatch::draw called outside begin/end pair");
    BatchedSprite bs;
    bs.position = sprite.getPosition();
    bs.size = { sprite.getGlobalBounds().width, sprite.getGlobalBounds().height };
    bs.rotation = sprite.getRotation();
    bs.color = sprite.getColor();
    bs.origin = sprite.getOrigin();
    bs.textureId = textureId;
    bs.depth = depth;
    m_sprites.push_back(bs);
    m_sorted = false;
}

void SpriteBatch::drawQuad(const sf::Vector2f& topLeft, const sf::Vector2f& topRight,
                           const sf::Vector2f& bottomLeft, const sf::Vector2f& bottomRight,
                           const sf::Color& color, int textureId,
                           const sf::Vector2f& texTopLeft,
                           const sf::Vector2f& texBottomRight) {
    assert(m_batching && "SpriteBatch::draw called outside begin/end pair");
    BatchedSprite bs;
    bs.position = topLeft;
    bs.size = { bottomRight.x - topLeft.x, bottomRight.y - topLeft.y };
    bs.color = color;
    bs.textureId = textureId;
    bs.blendMode = m_defaultBlend;

    SpriteBatchGlyph glyph;
    glyph.textureId = textureId;
    glyph.blendMode = m_defaultBlend;
    glyph.depth = 0.f;

    glyph.vertices[0] = sf::Vertex(topLeft, color, texTopLeft);
    glyph.vertices[1] = sf::Vertex(topRight, color, { texBottomRight.x, texTopLeft.y });
    glyph.vertices[2] = sf::Vertex(bottomRight, color, texBottomRight);
    glyph.vertices[3] = sf::Vertex(bottomLeft, color, { texTopLeft.x, texBottomRight.y });

    m_glyphs.push_back(glyph);
    m_sorted = false;
}

void SpriteBatch::flush() {
    if (!m_target || (m_sprites.empty() && m_glyphs.empty())) return;

    if (!m_sorted) sortByTextureThenDepth();

    for (const auto& sprite : m_sprites) {
        SpriteBatchGlyph glyph;
        glyph.textureId = sprite.textureId;
        glyph.blendMode = sprite.blendMode;
        glyph.depth = sprite.depth;

        float cosR = std::cos(sprite.rotation * 3.14159265f / 180.f);
        float sinR = std::sin(sprite.rotation * 3.14159265f / 180.f);
        float hw = sprite.size.x * 0.5f;
        float hh = sprite.size.y * 0.5f;

        sf::Vector2f corners[4] = {
            { -hw, -hh }, { hw, -hh }, { hw, hh }, { -hw, hh }
        };

        for (int i = 0; i < 4; ++i) {
            float rx = corners[i].x * cosR - corners[i].y * sinR;
            float ry = corners[i].x * sinR + corners[i].y * cosR;
            corners[i] = { sprite.position.x + rx + sprite.origin.x,
                           sprite.position.y + ry + sprite.origin.y };
        }

        sf::Vector2f texCoords[4] = {
            { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }
        };

        if (sprite.textureId >= 0) {
            auto texIt = m_textures.find(sprite.textureId);
            if (texIt != m_textures.end()) {
                auto atlasIt = m_atlases.find(sprite.textureId);
                if (atlasIt != m_atlases.end() && sprite.regionIndex < atlasIt->second.size()) {
                    const auto& region = atlasIt->second[sprite.regionIndex];
                    sf::Vector2u texSize = texIt->second.getSize();
                    float invW = 1.f / texSize.x;
                    float invH = 1.f / texSize.y;
                    texCoords[0] = { region.bounds.left * invW, region.bounds.top * invH };
                    texCoords[1] = { (region.bounds.left + region.bounds.width) * invW, region.bounds.top * invH };
                    texCoords[2] = { (region.bounds.left + region.bounds.width) * invW, (region.bounds.top + region.bounds.height) * invH };
                    texCoords[3] = { region.bounds.left * invW, (region.bounds.top + region.bounds.height) * invH };
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            glyph.vertices[i] = sf::Vertex(corners[i], sprite.color, texCoords[i]);
        }

        m_glyphs.push_back(glyph);
    }

    setupVertexArray();
    buildIndexBuffer(m_glyphs.size());

    int currentTexture = -1;
    sf::BlendMode currentBlend = m_defaultBlend;
    std::size_t batchStart = 0;

    for (std::size_t i = 0; i <= m_glyphs.size(); ++i) {
        bool needsFlush = (i == m_glyphs.size());
        if (!needsFlush) {
            needsFlush = (m_glyphs[i].textureId != currentTexture) ||
                         (std::memcmp(&m_glyphs[i].blendMode, &currentBlend, sizeof(sf::BlendMode)) != 0);
        }
        if (needsFlush) {
            std::size_t count = i - batchStart;
            if (count > 0) {
                sf::RenderStates states = m_baseStates;
                states.blendMode = currentBlend;
                if (currentTexture >= 0) {
                    auto texIt = m_textures.find(currentTexture);
                    if (texIt != m_textures.end()) {
                        states.texture = &texIt->second;
                    }
                }
                std::size_t vertexOffset = batchStart * 4;
                m_target->draw(&m_vertexBuffer[vertexOffset], count * 4, sf::Quads, states);
            }
            if (i < m_glyphs.size()) {
                currentTexture = m_glyphs[i].textureId;
                currentBlend = m_glyphs[i].blendMode;
                batchStart = i;
            }
        }
    }

    m_sprites.clear();
    m_glyphs.clear();
    m_vertexBuffer.clear();
    m_indexBuffer.clear();
}

void SpriteBatch::setTexture(int id, const sf::Texture& texture) {
    m_textures[id] = texture;
}

void SpriteBatch::removeTexture(int id) {
    m_textures.erase(id);
    m_atlases.erase(id);
}

bool SpriteBatch::hasTexture(int id) const {
    return m_textures.find(id) != m_textures.end();
}

const sf::Texture* SpriteBatch::getTexture(int id) const {
    auto it = m_textures.find(id);
    return (it != m_textures.end()) ? &it->second : nullptr;
}

int SpriteBatch::addAtlasRegion(int textureId, const AtlasRegion& region) {
    m_atlases[textureId].push_back(region);
    return static_cast<int>(m_atlases[textureId].size()) - 1;
}

const AtlasRegion* SpriteBatch::getAtlasRegion(int textureId, std::size_t index) const {
    auto it = m_atlases.find(textureId);
    if (it != m_atlases.end() && index < it->second.size()) {
        return &it->second[index];
    }
    return nullptr;
}

void SpriteBatch::setDefaultBlendMode(sf::BlendMode mode) {
    m_defaultBlend = mode;
}

sf::BlendMode SpriteBatch::getDefaultBlendMode() const {
    return m_defaultBlend;
}

void SpriteBatch::sortByTexture() {
    std::sort(m_sprites.begin(), m_sprites.end(),
        [](const BatchedSprite& a, const BatchedSprite& b) {
            return a.textureId < b.textureId;
        });
    m_sorted = true;
}

void SpriteBatch::sortByDepth() {
    std::sort(m_sprites.begin(), m_sprites.end(),
        [](const BatchedSprite& a, const BatchedSprite& b) {
            return a.depth < b.depth;
        });
    m_sorted = true;
}

void SpriteBatch::sortByTextureThenDepth() {
    std::sort(m_sprites.begin(), m_sprites.end(),
        [](const BatchedSprite& a, const BatchedSprite& b) {
            if (a.textureId != b.textureId) return a.textureId < b.textureId;
            return a.depth < b.depth;
        });
    m_sorted = true;
}

std::size_t SpriteBatch::getBatchCount() const {
    return m_glyphs.size();
}

std::size_t SpriteBatch::getSpriteCount() const {
    return m_sprites.size();
}

void SpriteBatch::clear() {
    m_sprites.clear();
    m_glyphs.clear();
    m_vertexBuffer.clear();
    m_indexBuffer.clear();
    m_textures.clear();
    m_atlases.clear();
    m_sorted = false;
}

void SpriteBatch::insertGlyph(const SpriteBatchGlyph& glyph) {
    m_glyphs.push_back(glyph);
}

void SpriteBatch::setupVertexArray() {
    m_vertexBuffer.resize(m_glyphs.size() * 4);
    for (std::size_t i = 0; i < m_glyphs.size(); ++i) {
        for (int j = 0; j < 4; ++j) {
            m_vertexBuffer[i * 4 + j] = m_glyphs[i].vertices[j];
        }
    }
}

void SpriteBatch::buildIndexBuffer(std::size_t spriteCount) {
    m_indexBuffer.resize(spriteCount * 6);
    for (std::size_t i = 0; i < spriteCount; ++i) {
        std::size_t base = i * 4;
        std::size_t idx = i * 6;
        m_indexBuffer[idx + 0] = static_cast<std::uint32_t>(base + 0);
        m_indexBuffer[idx + 1] = static_cast<std::uint32_t>(base + 1);
        m_indexBuffer[idx + 2] = static_cast<std::uint32_t>(base + 2);
        m_indexBuffer[idx + 3] = static_cast<std::uint32_t>(base + 0);
        m_indexBuffer[idx + 4] = static_cast<std::uint32_t>(base + 2);
        m_indexBuffer[idx + 5] = static_cast<std::uint32_t>(base + 3);
    }
}

} // namespace graphics
} // namespace engine
// v2: Added sort by texture
// v3: Batch optimization
