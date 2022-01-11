#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace engine {
namespace graphics {

struct WindowSettings {
    std::string title;
    sf::Vector2u resolution;
    sf::Vector2u minSize;
    sf::Vector2u maxSize;
    std::uint32_t style;
    sf::ContextSettings context;
    bool vsync;
    unsigned int framerateLimit;
    std::string iconFile;
    bool resizeable;

    WindowSettings()
        : title("Engine")
        , resolution(1280, 720)
        , minSize(640, 360)
        , maxSize(0, 0)
        , style(sf::Style::Default)
        , vsync(true)
        , framerateLimit(0)
        , resizeable(true) {}
};

class Window {
public:
    Window();
    explicit Window(const WindowSettings& settings);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    bool create(const WindowSettings& settings);
    bool create(const std::string& title, sf::Vector2u resolution,
                std::uint32_t style = sf::Style::Default);
    void close();

    bool isOpen() const;
    bool pollEvent(sf::Event& event);
    void clear(const sf::Color& color = sf::Color::Black);
    void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
    void draw(const sf::Vertex* vertices, std::size_t count, sf::PrimitiveType type,
              const sf::RenderStates& states = sf::RenderStates::Default);
    void display();

    void setTitle(const std::string& title);
    std::string getTitle() const;

    void setSize(const sf::Vector2u& size);
    sf::Vector2u getSize() const;

    void setPosition(const sf::Vector2i& position);
    sf::Vector2i getPosition() const;

    void setVSync(bool enabled);
    bool getVSync() const;

    void setFramerateLimit(unsigned int limit);
    unsigned int getFramerateLimit() const;

    void toggleFullscreen();
    bool isFullscreen() const;

    void setIcon(const std::string& filePath);
    void setIcon(const sf::Image& image);

    void setVisible(bool visible);
    bool isVisible() const;

    void setMouseCursorVisible(bool visible);
    void setMouseCursorGrabbed(bool grabbed);
    void setKeyRepeatEnabled(bool enabled);

    sf::RenderWindow* getNative();
    const sf::RenderWindow* getNative() const;
    operator sf::RenderWindow&();

    sf::Vector2f mapPixelToCoords(const sf::Vector2i& pixel) const;
    sf::Vector2i mapCoordsToPixel(const sf::Vector2f& coord) const;

    void setResizeCallback(std::function<void(sf::Vector2u)> callback);
    void setFocusCallback(std::function<void(bool)> callback);
    void setCloseCallback(std::function<bool()> callback);

    float getAspectRatio() const;
    sf::FloatRect getVisibleArea() const;
    void minimize();

private:
    void applySettings(const WindowSettings& settings);
    void handleEvents();

    std::unique_ptr<sf::RenderWindow> m_window;
    WindowSettings m_settings;
    sf::Image m_icon;

    std::function<void(sf::Vector2u)> m_resizeCallback;
    std::function<void(bool)> m_focusCallback;
    std::function<bool()> m_closeCallback;

    sf::Vector2u m_preFullscreenSize;
    sf::Vector2i m_preFullscreenPosition;
    bool m_fullscreen;
    bool m_created;
};

} // namespace graphics
} // namespace engine
