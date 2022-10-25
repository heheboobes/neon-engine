#include "TextureManager.h"
#include <cassert>
#include <sstream>

namespace engine {
namespace graphics {

TextureManager& TextureManager::instance() {
    static TextureManager inst;
    return inst;
}

TextureManager::TextureManager()
    : m_asyncRunning(false)
    , m_pendingCount(0)
    , m_fallbackColor(sf::Color::Magenta)
    , m_memoryTracking(true) {
    m_fallbackTexture = std::make_unique<sf::Texture>(
        createFallbackTexture(sf::Color::Magenta));
}

TextureManager::~TextureManager() {
    m_asyncRunning.store(false);
    for (auto& t : m_asyncThreads) {
        if (t.joinable()) t.join();
    }
    m_textures.clear();
}

const sf::Texture* TextureManager::load(const std::string& id, const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        it->second.info.referenceCount++;
        return it->second.texture.get();
    }

    TextureEntry entry;
    entry.texture = std::make_unique<sf::Texture>();
    if (!entry.texture->loadFromFile(filePath)) {
        return m_fallbackTexture.get();
    }

    entry.texture->setSmooth(false);
    entry.texture->setRepeated(false);

    entry.info.filePath = filePath;
    entry.info.size = entry.texture->getSize();
    entry.info.loaded = true;
    entry.info.smooth = false;
    entry.info.repeated = false;
    entry.info.referenceCount = 1;

    if (m_memoryTracking) {
        entry.info.memoryBytes = entry.info.size.x * entry.info.size.y * 4;
    }

    m_textures[id] = std::move(entry);
    return m_textures[id].texture.get();
}

const sf::Texture* TextureManager::get(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(id);
    if (it != m_textures.end() && it->second.info.loaded) {
        return it->second.texture.get();
    }
    return m_fallbackTexture.get();
}

bool TextureManager::exists(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_textures.find(id) != m_textures.end();
}

void TextureManager::unload(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        if (it->second.info.referenceCount > 1) {
            it->second.info.referenceCount--;
        } else {
            m_textures.erase(it);
        }
    }
}

void TextureManager::unloadAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_textures.clear();
}

void TextureManager::reloadAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_textures) {
        auto& entry = pair.second;
        if (!entry.info.filePath.empty()) {
            auto newTex = std::make_unique<sf::Texture>();
            if (newTex->loadFromFile(entry.info.filePath)) {
                newTex->setSmooth(entry.info.smooth);
                newTex->setRepeated(entry.info.repeated);
                entry.texture = std::move(newTex);
                entry.info.loaded = true;
            }
        }
    }
}

void TextureManager::reload(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        auto& entry = it->second;
        auto newTex = std::make_unique<sf::Texture>();
        if (newTex->loadFromFile(entry.info.filePath)) {
            newTex->setSmooth(entry.info.smooth);
            newTex->setRepeated(entry.info.repeated);
            entry.texture = std::move(newTex);
            entry.info.loaded = true;
        }
    }
}

void TextureManager::setSmooth(const std::string& id, bool smooth) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(id);
    if (it != m_textures.end() && it->second.texture) {
        it->second.texture->setSmooth(smooth);
        it->second.info.smooth = smooth;
    }
}

void TextureManager::setRepeated(const std::string& id, bool repeated) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(id);
    if (it != m_textures.end() && it->second.texture) {
        it->second.texture->setRepeated(repeated);
        it->second.info.repeated = repeated;
    }
}

void TextureManager::requestAsync(const std::string& id, const std::string& filePath,
                                  std::function<void(bool)> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    AsyncLoadRequest req;
    req.id = id;
    req.filePath = filePath;
    req.smooth = false;
    req.repeated = false;
    req.callback = std::move(callback);
    m_asyncQueue.push(std::move(req));
    m_pendingCount.store(m_asyncQueue.size());

    if (!m_asyncRunning.exchange(true)) {
        m_asyncThreads.emplace_back([this]() {
            while (true) {
                AsyncLoadRequest req;
                {
                    std::lock_guard<std::mutex> lk(m_mutex);
                    if (m_asyncQueue.empty()) {
                        m_asyncRunning.store(false);
                        return;
                    }
                    req = std::move(m_asyncQueue.front());
                    m_asyncQueue.pop();
                }
                sf::Texture tex;
                bool success = tex.loadFromFile(req.filePath);
                if (success) {
                    std::lock_guard<std::mutex> lk(m_mutex);
                    TextureEntry entry;
                    entry.texture = std::make_unique<sf::Texture>(std::move(tex));
                    entry.info.filePath = req.filePath;
                    entry.info.size = entry.texture->getSize();
                    entry.info.loaded = true;
                    entry.info.smooth = req.smooth;
                    entry.info.repeated = req.repeated;
                    entry.info.referenceCount = 1;
                    if (m_memoryTracking) {
                        entry.info.memoryBytes = entry.info.size.x * entry.info.size.y * 4;
                    }
                    m_textures[req.id] = std::move(entry);
                }
                if (req.callback) {
                    req.callback(success);
                }
                m_pendingCount.store(m_asyncQueue.size());
            }
        });
    }
}

void TextureManager::processAsyncQueue() {
    std::lock_guard<std::mutex> lock(m_mutex);
    while (!m_asyncQueue.empty()) {
        auto& req = m_asyncQueue.front();
        sf::Texture tex;
        bool success = tex.loadFromFile(req.filePath);
        if (success) {
            TextureEntry entry;
            entry.texture = std::make_unique<sf::Texture>(std::move(tex));
            entry.info.filePath = req.filePath;
            entry.info.size = entry.texture->getSize();
            entry.info.loaded = true;
            entry.info.referenceCount = 1;
            if (m_memoryTracking) {
                entry.info.memoryBytes = entry.info.size.x * entry.info.size.y * 4;
            }
            m_textures[req.id] = std::move(entry);
        }
        if (req.callback) {
            req.callback(success);
        }
        m_asyncQueue.pop();
    }
    m_pendingCount.store(0);
}

std::size_t TextureManager::pendingAsyncCount() const {
    return m_pendingCount.load();
}

TextureInfo TextureManager::getInfo(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        return it->second.info;
    }
    TextureInfo info;
    info.loaded = false;
    return info;
}

std::vector<std::string> TextureManager::getLoadedIds() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> ids;
    ids.reserve(m_textures.size());
    for (const auto& pair : m_textures) {
        if (pair.second.info.loaded) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

std::size_t TextureManager::getTotalMemoryBytes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::size_t total = 0;
    for (const auto& pair : m_textures) {
        total += pair.second.info.memoryBytes;
    }
    return total;
}

std::size_t TextureManager::getTextureCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_textures.size();
}

void TextureManager::setFallbackColor(const sf::Color& color) {
    m_fallbackColor = color;
    m_fallbackTexture = std::make_unique<sf::Texture>(createFallbackTexture(color));
}

const sf::Texture* TextureManager::getFallbackTexture() const {
    return m_fallbackTexture.get();
}

void TextureManager::enableMemoryTracking(bool enabled) {
    m_memoryTracking = enabled;
}

bool TextureManager::isMemoryTrackingEnabled() const {
    return m_memoryTracking;
}

sf::Texture TextureManager::createFallbackTexture(const sf::Color& color) const {
    sf::Image img;
    img.create(4, 4, color);
    sf::Texture tex;
    tex.loadFromImage(img);
    tex.setRepeated(true);
    return tex;
}

} // namespace graphics
} // namespace engine
// v2: Thread safety fix


