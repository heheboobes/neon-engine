#include "Renderer.h"
#include <algorithm>
#include <cassert>

namespace engine {
namespace graphics {

Renderer& Renderer::instance() {
    static Renderer inst;
    return inst;
}

void Renderer::initialize(sf::RenderWindow& window) {
    m_window = &window;
    m_defaultView = window.getDefaultView();
    m_currentView = m_defaultView;
    m_initialized = true;
    m_vsync = true;
}

void Renderer::shutdown() {
    m_renderPasses.clear();
    m_viewports.clear();
    while (!m_stateStack.empty()) m_stateStack.pop();
    m_window = nullptr;
    m_initialized = false;
}

void Renderer::clear(const sf::Color& color) {
    assert(m_window && "Renderer not initialized");
    if (m_activeViewport >= 0 && m_activeViewport < static_cast<int>(m_viewports.size())) {
        const auto& vp = m_viewports[m_activeViewport];
        if (vp.config.clearOnRender) {
            sf::FloatRect vpRect = vp.config.rect;
            sf::View vpView(vpRect);
            vpView.setViewport(vp.config.rect);
            m_window->setView(vpView);
            m_window->clear(vp.config.clearColor);
        }
    } else {
        m_window->clear(color);
    }
}

void Renderer::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
    assert(m_window && "Renderer not initialized");
    sf::RenderStates finalStates = states;
    if (m_viewModified) {
        m_window->setView(m_currentView);
        m_viewModified = false;
    }
    if (!m_stateStack.empty()) {
        finalStates = m_stateStack.top();
    }
    m_window->draw(drawable, finalStates);
}

void Renderer::draw(const sf::Vertex* vertices, std::size_t count, sf::PrimitiveType type, const sf::RenderStates& states) {
    assert(m_window && "Renderer not initialized");
    sf::RenderStates finalStates = states;
    if (m_viewModified) {
        m_window->setView(m_currentView);
        m_viewModified = false;
    }
    if (!m_stateStack.empty()) {
        finalStates = m_stateStack.top();
    }
    m_window->draw(vertices, count, type, finalStates);
}

void Renderer::display() {
    assert(m_window && "Renderer not initialized");
    for (auto& pass : m_renderPasses) {
        if (pass.enabled) {
            pass.callback(*m_window);
        }
    }
    m_window->display();
}

void Renderer::setView(const sf::View& view) {
    m_currentView = view;
    m_viewModified = true;
}

const sf::View& Renderer::getView() const {
    return m_currentView;
}

void Renderer::resetView() {
    m_currentView = m_defaultView;
    m_viewModified = true;
}

int Renderer::addViewport(const ViewportConfig& config) {
    ViewportEntry entry;
    entry.id = m_nextViewportId++;
    entry.config = config;
    m_viewports.push_back(entry);
    return entry.id;
}

void Renderer::removeViewport(int id) {
    auto it = std::remove_if(m_viewports.begin(), m_viewports.end(),
        [id](const ViewportEntry& e) { return e.id == id; });
    m_viewports.erase(it, m_viewports.end());
    if (m_activeViewport == id) m_activeViewport = -1;
}

void Renderer::setActiveViewport(int id) {
    for (std::size_t i = 0; i < m_viewports.size(); ++i) {
        if (m_viewports[i].id == id) {
            m_activeViewport = id;
            const auto& vp = m_viewports[i];
            sf::View vpView(vp.config.rect);
            vpView.setViewport(vp.config.rect);
            m_window->setView(vpView);
            return;
        }
    }
    m_activeViewport = -1;
}

int Renderer::currentViewport() const {
    return m_activeViewport;
}

void Renderer::addRenderPass(RenderPassType type, const std::string& name, int priority,
                             std::function<void(sf::RenderTarget&)> callback) {
    RenderPass pass;
    pass.type = type;
    pass.name = name;
    pass.priority = priority;
    pass.callback = callback;
    pass.enabled = true;
    m_renderPasses.push_back(pass);
    std::sort(m_renderPasses.begin(), m_renderPasses.end(),
        [](const RenderPass& a, const RenderPass& b) {
            if (a.type != b.type) return static_cast<int>(a.type) < static_cast<int>(b.type);
            return a.priority < b.priority;
        });
}

void Renderer::removeRenderPass(const std::string& name) {
    auto it = std::remove_if(m_renderPasses.begin(), m_renderPasses.end(),
        [&name](const RenderPass& p) { return p.name == name; });
    m_renderPasses.erase(it, m_renderPasses.end());
}

void Renderer::setRenderPassEnabled(const std::string& name, bool enabled) {
    for (auto& pass : m_renderPasses) {
        if (pass.name == name) {
            pass.enabled = enabled;
            return;
        }
    }
}

void Renderer::executeRenderPasses(sf::RenderTarget& target) {
    for (auto& pass : m_renderPasses) {
        if (pass.enabled) {
            pass.callback(target);
        }
    }
}

void Renderer::pushStates(const sf::RenderStates& states) {
    m_stateStack.push(states);
}

void Renderer::popStates() {
    if (!m_stateStack.empty()) m_stateStack.pop();
}

std::size_t Renderer::stateStackSize() const {
    return m_stateStack.size();
}

sf::RenderTarget* Renderer::getCurrentTarget() const {
    return m_window;
}

sf::RenderWindow* Renderer::getWindow() const {
    return m_window;
}

sf::Vector2u Renderer::getSize() const {
    assert(m_window && "Renderer not initialized");
    return m_window->getSize();
}

sf::FloatRect Renderer::getVisibleArea() const {
    assert(m_window && "Renderer not initialized");
    sf::View current = m_window->getView();
    sf::Vector2u size = m_window->getSize();
    sf::FloatRect viewport = current.getViewport();
    float factorX = static_cast<float>(size.x) / viewport.width;
    float factorY = static_cast<float>(size.y) / viewport.height;
    sf::Vector2f center = current.getCenter();
    sf::Vector2f halfSize = current.getSize() * 0.5f;
    return sf::FloatRect(center.x - halfSize.x, center.y - halfSize.y,
                         current.getSize().x, current.getSize().y);
}

void Renderer::setVSync(bool enabled) {
    assert(m_window && "Renderer not initialized");
    m_window->setVerticalSyncEnabled(enabled);
    m_vsync = enabled;
}

bool Renderer::getVSync() const {
    return m_vsync;
}

} // namespace graphics
} // namespace engine
// v2: Added render pass system
