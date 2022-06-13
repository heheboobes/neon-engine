#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>

namespace engine {
namespace graphics {

struct TextureInfo {
    std::string filePath;
    sf::Vector2u size;
    std::size_t memoryBytes;
    bool loaded;
    bool smooth;
    bool repeated;
    unsigned int referenceCount;

    TextureInfo()
        : memoryBytes(0)
        , loaded(false)
        , smooth(false)
        , repeated(false)
        , referenceCount(0) {}
};

struct AsyncLoadRequest {
    std::string id;
    std::string filePath;
    bool smooth;
    bool repeated;
    std::function<void(bool)> callback;
};

class TextureManager {
public:
    static TextureManager& instance();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    const sf::Texture* load(const std::string& id, const std::string& filePath);
    const sf::Texture* get(const std::string& id) const;
    bool exists(const std::string& id) const;

    void unload(const std::string& id);
    void unloadAll();
    void reloadAll();
    void reload(const std::string& id);

    void setSmooth(const std::string& id, bool smooth);
    void setRepeated(const std::string& id, bool repeated);

    void requestAsync(const std::string& id, const std::string& filePath,
                      std::function<void(bool)> callback = nullptr);
    void processAsyncQueue();
    std::size_t pendingAsyncCount() const;

    TextureInfo getInfo(const std::string& id) const;
    std::vector<std::string> getLoadedIds() const;
    std::size_t getTotalMemoryBytes() const;
    std::size_t getTextureCount() const;

    void setFallbackColor(const sf::Color& color);
    const sf::Texture* getFallbackTexture() const;

    void enableMemoryTracking(bool enabled);
    bool isMemoryTrackingEnabled() const;

private:
    TextureManager();
    ~TextureManager();

    struct TextureEntry {
        std::unique_ptr<sf::Texture> texture;
        TextureInfo info;
    };

    sf::Texture createFallbackTexture(const sf::Color& color) const;

    mutable std::mutex m_mutex;
    std::unordered_map<std::string, TextureEntry> m_textures;
    std::unique_ptr<sf::Texture> m_fallbackTexture;

    std::queue<AsyncLoadRequest> m_asyncQueue;
    std::vector<std::thread> m_asyncThreads;
    std::atomic<bool> m_asyncRunning;
    std::atomic<std::size_t> m_pendingCount;

    sf::Color m_fallbackColor;
    bool m_memoryTracking;
};

} // namespace graphics
} // namespace engine
