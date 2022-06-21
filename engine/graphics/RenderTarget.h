#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace engine {
namespace graphics {

enum class TargetType : uint8_t {
    Window,
    RenderTexture,
    Null
};

enum class ClearFlags : uint8_t {
    None = 0,
    Color = 1 << 0,
    Depth = 1 << 1,
    Stencil = 1 << 2,
    All = Color | Depth | Stencil
};

inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
    return static_cast<ClearFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline ClearFlags operator&(ClearFlags a, ClearFlags b) {
    return static_cast<ClearFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

struct RenderTargetDesc {
    sf::Vector2u size;
    std::string name;
    bool useDepthBuffer;
    bool useStencilBuffer;
    bool smooth;
    bool repeated;
    bool generateMipmaps;
    sf::Color defaultClearColor;

    RenderTargetDesc()
        : size(256, 256)
        , name("unnamed")
        , useDepthBuffer(true)
        , useStencilBuffer(false)
        , smooth(false)
        , repeated(false)
        , generateMipmaps(false)
        , defaultClearColor(sf::Color::Transparent) {}
};

class RenderTarget {
public:
    RenderTarget();
    explicit RenderTarget(const RenderTargetDesc& desc);
    ~RenderTarget();

    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;
    RenderTarget(RenderTarget&& other) noexcept;
    RenderTarget& operator=(RenderTarget&& other) noexcept;

    bool create(const RenderTargetDesc& desc);
    bool create(sf::Vector2u size, const std::string& name = "rt");

    void clear(const sf::Color& color = sf::Color::Transparent, ClearFlags flags = ClearFlags::All);
    void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
    void draw(const sf::Vertex* vertices, std::size_t count, sf::PrimitiveType type,
              const sf::RenderStates& states = sf::RenderStates::Default);
    void display();

    void setView(const sf::View& view);
    const sf::View& getView() const;
    void setDefaultView();
    sf::View getDefaultView() const;

    void setActive(bool active = true);

    sf::Vector2u getSize() const;
    sf::FloatRect getVisibleArea() const;
    TargetType getType() const;

    sf::RenderTarget* getNativeTarget();
    const sf::RenderTarget* getNativeTarget() const;

    sf::RenderTexture* getRenderTexture();
    const sf::RenderTexture* getRenderTexture() const;
    const sf::Texture& getTexture() const;

    bool isValid() const;
    void release();

    void setSmooth(bool smooth);
    bool isSmooth() const;

    void setRepeated(bool repeated);
    bool isRepeated() const;

    void setName(const std::string& name);
    std::string getName() const;

    RenderTargetDesc getDescription() const;

    bool saveToFile(const std::string& filePath);

    void pushStates(const sf::RenderStates& states);
    void popStates();

    void setBlendMode(sf::BlendMode mode);
    sf::BlendMode getBlendMode() const;

private:
    std::unique_ptr<sf::RenderTexture> m_renderTexture;
    sf::View m_defaultView;
    sf::View m_currentView;
    TargetType m_type;
    RenderTargetDesc m_desc;
    sf::BlendMode m_blendMode;
    std::vector<sf::RenderStates> m_stateStack;
    bool m_valid;
};

} // namespace graphics
} // namespace engine
