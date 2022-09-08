#include "TileSet.h"

namespace engine::tilemap {

TileCollision::TileCollision()
    : solid(false)
    , oneWay(false)
    , flags(0)
{
}

TileSet::TileSet()
    : m_texture(nullptr)
    , m_ownsTexture(false)
    , m_tileWidth(16)
    , m_tileHeight(16)
    , m_spacing(0)
    , m_margin(0)
    , m_columns(0)
    , m_rows(0)
    , m_firstGID(1)
{
}

bool TileSet::loadFromFile(const std::string& texturePath, int tileWidth, int tileHeight,
                           int spacing, int margin) {
    auto* tex = new sf::Texture();
    if (!tex->loadFromFile(texturePath)) {
        delete tex;
        return false;
    }

    if (m_ownsTexture) {
        delete m_texture;
    }

    m_texture = tex;
    m_ownsTexture = true;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_spacing = spacing;
    m_margin = margin;

    int texW = static_cast<int>(m_texture->getSize().x) - 2 * margin;
    int texH = static_cast<int>(m_texture->getSize().y) - 2 * margin;
    m_columns = texW / (tileWidth + spacing);
    m_rows = texH / (tileHeight + spacing);

    return true;
}

bool TileSet::loadFromTexture(const sf::Texture& texture, int tileWidth, int tileHeight,
                              int spacing, int margin) {
    m_texture = &texture;
    m_ownsTexture = false;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_spacing = spacing;
    m_margin = margin;

    int texW = static_cast<int>(m_texture->getSize().x) - 2 * margin;
    int texH = static_cast<int>(m_texture->getSize().y) - 2 * margin;
    m_columns = texW / (tileWidth + spacing);
    m_rows = texH / (tileHeight + spacing);

    return true;
}

int TileSet::getTileWidth() const { return m_tileWidth; }
int TileSet::getTileHeight() const { return m_tileHeight; }
int TileSet::getColumns() const { return m_columns; }
int TileSet::getRows() const { return m_rows; }
int TileSet::getTileCount() const { return m_columns * m_rows; }
int TileSet::getSpacing() const { return m_spacing; }
int TileSet::getMargin() const { return m_margin; }

sf::IntRect TileSet::getTileTextureRect(int tileIndex) const {
    int col = tileIndex % m_columns;
    int row = tileIndex / m_columns;
    return sf::IntRect(
        m_margin + col * (m_tileWidth + m_spacing),
        m_margin + row * (m_tileHeight + m_spacing),
        m_tileWidth, m_tileHeight);
}

const sf::Texture* TileSet::getTexture() const { return m_texture; }

void TileSet::setCollision(int tileIndex, bool solid, unsigned int flags) {
    m_collisionMap[tileIndex] = TileCollision();
    m_collisionMap[tileIndex].solid = solid;
    m_collisionMap[tileIndex].flags = flags;
}

const TileCollision& TileSet::getCollision(int tileIndex) const {
    static TileCollision defaultCol;
    auto it = m_collisionMap.find(tileIndex);
    if (it != m_collisionMap.end()) return it->second;
    return defaultCol;
}

bool TileSet::isTileSolid(int tileIndex) const {
    auto it = m_collisionMap.find(tileIndex);
    return it != m_collisionMap.end() && it->second.solid;
}

void TileSet::addAnimation(int tileIndex, const TileAnimation& anim) {
    m_animations[tileIndex] = anim;
}

bool TileSet::hasAnimation(int tileIndex) const {
    return m_animations.find(tileIndex) != m_animations.end();
}

const TileAnimation& TileSet::getAnimation(int tileIndex) const {
    static TileAnimation defaultAnim;
    defaultAnim.currentFrame = 0;
    defaultAnim.looping = true;
    defaultAnim.timer = 0.f;
    auto it = m_animations.find(tileIndex);
    if (it != m_animations.end()) return it->second;
    return defaultAnim;
}

TileAnimationFrame TileSet::getAnimatedTile(int tileIndex, float dt) {
    auto it = m_animations.find(tileIndex);
    if (it == m_animations.end()) {
        return TileAnimationFrame{tileIndex, 0.f};
    }

    auto& anim = it->second;
    if (anim.frames.empty()) {
        return TileAnimationFrame{tileIndex, 0.f};
    }

    anim.timer += dt;
    const auto& frame = anim.frames[anim.currentFrame];

    if (anim.timer >= frame.duration) {
        anim.timer -= frame.duration;
        anim.currentFrame++;
        if (anim.currentFrame >= static_cast<int>(anim.frames.size())) {
            if (anim.looping) {
                anim.currentFrame = 0;
            } else {
                anim.currentFrame = static_cast<int>(anim.frames.size()) - 1;
            }
        }
    }

    return anim.frames[anim.currentFrame];
}

int TileSet::getFirstGID() const { return m_firstGID; }
void TileSet::setFirstGID(int gid) { m_firstGID = gid; }

} // namespace engine::tilemap
