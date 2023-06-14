#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <stack>

namespace engine {
namespace graphics {

enum class RenderPassType : uint8_t {
    Opaque,
    Transparent,
    PostProcess,
    Overlay,
    Count
};

struct RenderPass {
    RenderPassType type;
    std::string name;
    std::function<void(sf::RenderTarget&)> callback;
    int priority;
    bool enabled;
    sf::RenderStates states;

    RenderPass()
        : type(RenderPassType::Opaque)
        , name("unnamed")
        , priority(0)
        , enabled(true)
        , states(sf::BlendAlpha) {}
};

struct ViewportConfig {
    sf::FloatRect rect;
    bool clearOnRender;
    sf::Color clearColor;
    sf::View view;

    ViewportConfig()
        : rect(0.f, 0.f, 1.f, 1.f)
        , clearOnRender(true)
        , clearColor(sf::Color::Black) {}
};

class Renderer {
public:
    static Renderer& instance();

    void initialize(sf::RenderWindow& window);
    void shutdown();

    void clear(const sf::Color& color = sf::Color::Black);
    void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
    void draw(const sf::Vertex* vertices, std::size_t count, sf::PrimitiveType type, const sf::RenderStates& states = sf::RenderStates::Default);
    void display();

    void setView(const sf::View& view);
    const sf::View& getView() const;
    void resetView();

    int addViewport(const ViewportConfig& config);
    void removeViewport(int id);
    void setActiveViewport(int id);
    int currentViewport() const;

    void addRenderPass(RenderPassType type, const std::string& name, int priority,
                       std::function<void(sf::RenderTarget&)> callback);
    void removeRenderPass(const std::string& name);
    void setRenderPassEnabled(const std::string& name, bool enabled);
    void executeRenderPasses(sf::RenderTarget& target);

    void pushStates(const sf::RenderStates& states);
    void popStates();
    std::size_t stateStackSize() const;

    sf::RenderTarget* getCurrentTarget() const;
    sf::RenderWindow* getWindow() const;

    sf::Vector2u getSize() const;
    sf::FloatRect getVisibleArea() const;

    void setVSync(bool enabled);
    bool getVSync() const;

private:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    struct ViewportEntry {
        int id;
        ViewportConfig config;
    };

    sf::RenderWindow* m_window = nullptr;
    sf::View m_defaultView;
    sf::View m_currentView;
    bool m_viewModified = false;

    std::vector<ViewportEntry> m_viewports;
    int m_activeViewport = -1;
    int m_nextViewportId = 0;

    std::vector<RenderPass> m_renderPasses;
    std::stack<sf::RenderStates> m_stateStack;

    bool m_initialized = false;
    bool m_vsync = true;
};

} // namespace graphics
} // namespace engine
// DOCS: Graphics API docs
