#include "Window.h"
#include <cassert>
#include <algorithm>

namespace engine {
namespace graphics {

Window::Window()
    : m_window(std::make_unique<sf::RenderWindow>())
    , m_fullscreen(false)
    , m_created(false) {}

Window::Window(const WindowSettings& settings)
    : m_window(std::make_unique<sf::RenderWindow>())
    , m_fullscreen(false)
    , m_created(false) {
    create(settings);
}

Window::~Window() {
    close();
}

Window::Window(Window&& other) noexcept
    : m_window(std::move(other.m_window))
    , m_settings(other.m_settings)
    , m_icon(std::move(other.m_icon))
    , m_resizeCallback(std::move(other.m_resizeCallback))
    , m_focusCallback(std::move(other.m_focusCallback))
    , m_closeCallback(std::move(other.m_closeCallback))
    , m_preFullscreenSize(other.m_preFullscreenSize)
    , m_preFullscreenPosition(other.m_preFullscreenPosition)
    , m_fullscreen(other.m_fullscreen)
    , m_created(other.m_created) {
    other.m_created = false;
}

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        m_window = std::move(other.m_window);
        m_settings = other.m_settings;
        m_icon = std::move(other.m_icon);
        m_resizeCallback = std::move(other.m_resizeCallback);
        m_focusCallback = std::move(other.m_focusCallback);
        m_closeCallback = std::move(other.m_closeCallback);
        m_preFullscreenSize = other.m_preFullscreenSize;
        m_preFullscreenPosition = other.m_preFullscreenPosition;
        m_fullscreen = other.m_fullscreen;
        m_created = other.m_created;
        other.m_created = false;
    }
    return *this;
}

bool Window::create(const WindowSettings& settings) {
    if (m_created) close();

    m_settings = settings;
    std::uint32_t style = settings.style;

    if (settings.resizeable) {
        style |= sf::Style::Resize;
    } else {
        style &= ~sf::Style::Resize;
    }

    m_window->create(
        sf::VideoMode(settings.resolution.x, settings.resolution.y),
        settings.title,
        style,
        settings.context
    );

    if (settings.minSize.x > 0 && settings.minSize.y > 0) {
        m_window->setSize(settings.minSize);
    }

    applySettings(settings);

    if (!settings.iconFile.empty()) {
        setIcon(settings.iconFile);
    }

    m_created = true;
    return true;
}

bool Window::create(const std::string& title, sf::Vector2u resolution,
                    std::uint32_t style) {
    WindowSettings settings;
    settings.title = title;
    settings.resolution = resolution;
    settings.style = style;
    return create(settings);
}

void Window::close() {
    if (m_created && m_window) {
        m_window->close();
        m_created = false;
    }
}

bool Window::isOpen() const {
    return m_window && m_window->isOpen();
}

bool Window::pollEvent(sf::Event& event) {
    if (!m_window) return false;

    bool hasEvent = m_window->pollEvent(event);
    if (hasEvent) {
        if (event.type == sf::Event::Resized && m_resizeCallback) {
            m_resizeCallback({ event.size.width, event.size.height });
        } else if (event.type == sf::Event::GainedFocus && m_focusCallback) {
            m_focusCallback(true);
        } else if (event.type == sf::Event::LostFocus && m_focusCallback) {
            m_focusCallback(false);
        } else if (event.type == sf::Event::Closed) {
            if (m_closeCallback) {
                if (!m_closeCallback()) {
                    hasEvent = false;
                }
            }
        }
    }
    return hasEvent;
}

void Window::clear(const sf::Color& color) {
    if (m_window) m_window->clear(color);
}

void Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
    if (m_window) m_window->draw(drawable, states);
}

void Window::draw(const sf::Vertex* vertices, std::size_t count, sf::PrimitiveType type,
                  const sf::RenderStates& states) {
    if (m_window) m_window->draw(vertices, count, type, states);
}

void Window::display() {
    if (m_window) m_window->display();
}

void Window::setTitle(const std::string& title) {
    m_settings.title = title;
    if (m_window) m_window->setTitle(title);
}

std::string Window::getTitle() const {
    return m_settings.title;
}

void Window::setSize(const sf::Vector2u& size) {
    m_settings.resolution = size;
    if (m_window) m_window->setSize(size);
}

sf::Vector2u Window::getSize() const {
    return m_window ? m_window->getSize() : m_settings.resolution;
}

void Window::setPosition(const sf::Vector2i& position) {
    if (m_window) m_window->setPosition(position);
}

sf::Vector2i Window::getPosition() const {
    return m_window ? m_window->getPosition() : sf::Vector2i(0, 0);
}

void Window::setVSync(bool enabled) {
    m_settings.vsync = enabled;
    if (m_window) m_window->setVerticalSyncEnabled(enabled);
}

bool Window::getVSync() const {
    return m_settings.vsync;
}

void Window::setFramerateLimit(unsigned int limit) {
    m_settings.framerateLimit = limit;
    if (m_window) m_window->setFramerateLimit(limit);
}

unsigned int Window::getFramerateLimit() const {
    return m_settings.framerateLimit;
}

void Window::toggleFullscreen() {
    if (!m_window) return;

    if (m_fullscreen) {
        m_window->create(
            sf::VideoMode(m_preFullscreenSize.x, m_preFullscreenSize.y),
            m_settings.title,
            m_settings.style & ~sf::Style::Fullscreen,
            m_settings.context
        );
        m_window->setPosition(m_preFullscreenPosition);
        m_fullscreen = false;
    } else {
        m_preFullscreenSize = m_window->getSize();
        m_preFullscreenPosition = m_window->getPosition();
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        m_window->create(
            desktop,
            m_settings.title,
            sf::Style::Fullscreen,
            m_settings.context
        );
        m_fullscreen = true;
    }

    applySettings(m_settings);
    if (!m_settings.iconFile.empty()) {
        setIcon(m_settings.iconFile);
    }
}

bool Window::isFullscreen() const {
    return m_fullscreen;
}

void Window::setIcon(const std::string& filePath) {
    m_settings.iconFile = filePath;
    m_icon.loadFromFile(filePath);
    if (m_window && m_icon.getSize().x > 0) {
        m_window->setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());
    }
}

void Window::setIcon(const sf::Image& image) {
    m_icon = image;
    if (m_window && m_icon.getSize().x > 0) {
        m_window->setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());
    }
}

void Window::setVisible(bool visible) {
    if (m_window) m_window->setVisible(visible);
}

bool Window::isVisible() const {
    return m_window ? m_window->isOpen() : false;
}

void Window::setMouseCursorVisible(bool visible) {
    if (m_window) m_window->setMouseCursorVisible(visible);
}

void Window::setMouseCursorGrabbed(bool grabbed) {
    if (m_window) m_window->setMouseCursorGrabbed(grabbed);
}

void Window::setKeyRepeatEnabled(bool enabled) {
    if (m_window) m_window->setKeyRepeatEnabled(enabled);
}

sf::RenderWindow* Window::getNative() {
    return m_window.get();
}

const sf::RenderWindow* Window::getNative() const {
    return m_window.get();
}

Window::operator sf::RenderWindow&() {
    return *m_window;
}

sf::Vector2f Window::mapPixelToCoords(const sf::Vector2i& pixel) const {
    return m_window ? m_window->mapPixelToCoords(pixel) : sf::Vector2f();
}

sf::Vector2i Window::mapCoordsToPixel(const sf::Vector2f& coord) const {
    return m_window ? m_window->mapCoordsToPixel(coord) : sf::Vector2i();
}

void Window::setResizeCallback(std::function<void(sf::Vector2u)> callback) {
    m_resizeCallback = std::move(callback);
}

void Window::setFocusCallback(std::function<void(bool)> callback) {
    m_focusCallback = std::move(callback);
}

void Window::setCloseCallback(std::function<bool()> callback) {
    m_closeCallback = std::move(callback);
}

float Window::getAspectRatio() const {
    sf::Vector2u size = getSize();
    return (size.y > 0) ? static_cast<float>(size.x) / static_cast<float>(size.y) : 1.f;
}

sf::FloatRect Window::getVisibleArea() const {
    if (!m_window) return sf::FloatRect();
    sf::View current = m_window->getView();
    sf::Vector2u size = m_window->getSize();
    sf::FloatRect viewport = current.getViewport();
    sf::Vector2f halfSize = current.getSize() * 0.5f;
    sf::Vector2f center = current.getCenter();
    float factorX = static_cast<float>(size.x) / viewport.width;
    float factorY = static_cast<float>(size.y) / viewport.height;
    return sf::FloatRect(center.x - halfSize.x, center.y - halfSize.y,
                         current.getSize().x, current.getSize().y);
}

void Window::minimize() {
    if (m_window) m_window->setVisible(false);
}

void Window::applySettings(const WindowSettings& settings) {
    m_window->setVerticalSyncEnabled(settings.vsync);
    if (settings.framerateLimit > 0) {
        m_window->setFramerateLimit(settings.framerateLimit);
    }
    if (settings.minSize.x > 0 && settings.minSize.y > 0) {
        m_window->setSize(settings.minSize);
    }
}

void Window::handleEvents() {
    sf::Event event;
    while (pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            if (m_closeCallback) {
                if (!m_closeCallback()) continue;
            }
            close();
        }
    }
}

} // namespace graphics
} // namespace engine
// v2: Improved focus handling
