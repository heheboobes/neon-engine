#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace engine {
namespace graphics {

struct FontInfo {
    std::string filePath;
    std::size_t memoryBytes;
    unsigned int size;
    bool loaded;
    bool smooth;
    unsigned int referenceCount;
    std::vector<std::string> fallbackChain;

    FontInfo()
        : memoryBytes(0)
        , size(12)
        , loaded(false)
        , smooth(true)
        , referenceCount(0) {}
};

struct FontFallback {
    std::string familyName;
    std::string filePath;
    int priority;

    FontFallback() : priority(0) {}
};

class FontManager {
public:
    static FontManager& instance();

    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    const sf::Font* load(const std::string& id, const std::string& filePath, unsigned int size = 12);
    const sf::Font* get(const std::string& id) const;
    bool exists(const std::string& id) const;

    void unload(const std::string& id);
    void unloadAll();
    void reloadAll();

    FontInfo getInfo(const std::string& id) const;
    std::vector<std::string> getLoadedIds() const;
    std::size_t getFontCount() const;

    void addFallback(const std::string& fontId, const std::string& fallbackId);
    void removeFallback(const std::string& fontId, const std::string& fallbackId);
    std::vector<std::string> getFallbacks(const std::string& fontId) const;
    void clearFallbacks(const std::string& fontId);

    const sf::Font* resolve(const std::string& id) const;
    const sf::Font* resolveWithFallback(const std::string& id, wchar_t character) const;
    bool hasGlyph(const sf::Font& font, wchar_t character) const;

    void setDefaultFont(const std::string& id);
    const sf::Font* getDefaultFont() const;
    bool hasDefaultFont() const;

    void enableSmoothing(const std::string& id, bool smooth);
    bool isSmoothingEnabled(const std::string& id) const;

    void setGlobalFallback(const std::string& fontId);
    const sf::Font* getGlobalFallback() const;

    std::vector<FontFallback> getRegisteredFallbacks() const;
    void registerFallbackFamily(const std::string& family, const std::string& filePath, int priority = 0);

    std::size_t getTotalMemoryBytes() const;

private:
    FontManager();
    ~FontManager();

    struct FontEntry {
        std::unique_ptr<sf::Font> font;
        FontInfo info;
    };

    mutable std::unique_ptr<sf::Font> m_defaultFont;
    std::string m_defaultFontId;
    bool m_hasDefaultFont;

    mutable std::unique_ptr<sf::Font> m_globalFallback;
    std::string m_globalFallbackId;

    std::unordered_map<std::string, FontEntry> m_fonts;
    std::unordered_map<std::string, std::vector<std::string>> m_fallbackChains;
    std::vector<FontFallback> m_registeredFallbacks;

    sf::Font createDefaultFont() const;
};

} // namespace graphics
} // namespace engine
