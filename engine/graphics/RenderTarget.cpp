#include "RenderTarget.h"
#include <cassert>
#include <algorithm>

namespace engine {
namespace graphics {

RenderTarget::RenderTarget()
    : m_type(TargetType::Null)
    , m_blendMode(sf::BlendAlpha)
    , m_valid(false) {}

RenderTarget::RenderTarget(const RenderTargetDesc& desc)
    : m_type(TargetType::Null)
    , m_blendMode(sf::BlendAlpha)
    , m_valid(false) {
    create(desc);
}

RenderTarget::~RenderTarget() {
    release();
}

RenderTarget::RenderTarget(RenderTarget&& other) noexcept
    : m_renderTexture(std::move(other.m_renderTexture))
    , m_defaultView(other.m_defaultView)
    , m_currentView(other.m_currentView)
    , m_type(other.m_type)
    , m_desc(other.m_desc)
    , m_blendMode(other.m_blendMode)
    , m_stateStack(std::move(other.m_stateStack))
    , m_valid(other.m_valid) {
    other.m_type = TargetType::Null;
    other.m_valid = false;
}

RenderTarget& RenderTarget::operator=(RenderTarget&& other) noexcept {
    if (this != &other) {
        release();
        m_renderTexture = std::move(other.m_renderTexture);
        m_defaultView = other.m_defaultView;
        m_currentView = other.m_currentView;
        m_type = other.m_type;
        m_desc = other.m_desc;
        m_blendMode = other.m_blendMode;
        m_stateStack = std::move(other.m_stateStack);
        m_valid = other.m_valid;
        other.m_type = TargetType::Null;
        other.m_valid = false;
    }
    return *this;
}

bool RenderTarget::create(const RenderTargetDesc& desc) {
    release();
    m_desc = desc;
    m_renderTexture = std::make_unique<sf::RenderTexture>();

    sf::ContextSettings ctx;
    ctx.depthBits = desc.useDepthBuffer ? 24 : 0;
    ctx.stencilBits = desc.useStencilBuffer ? 8 : 0;

    bool success = m_renderTexture->create(desc.size.x, desc.size.y, ctx);
    if (!success) {
        m_renderTexture.reset();
        m_type = TargetType::Null;
        m_valid = false;
        return false;
    }

    m_renderTexture->setSmooth(desc.smooth);
    m_renderTexture->setRepeated(desc.repeated);
    if (desc.generateMipmaps) {
        m_renderTexture->generateMipmap();
    }

    m_defaultView = m_renderTexture->getDefaultView();
    m_currentView = m_defaultView;
    m_type = TargetType::RenderTexture;
    m_valid = true;

    clear(desc.defaultClearColor);
    display();

    return true;
}

bool RenderTarget::create(sf::Vector2u size, const std::string& name) {
    RenderTargetDesc desc;
    desc.size = size;
    desc.name = name;
    return create(desc);
}

void RenderTarget::clear(const sf::Color& color, ClearFlags flags) {
    if (!m_valid || !m_renderTexture) return;

    sf::RenderTexture& rt = *m_renderTexture;
    if ((flags & ClearFlags::Color) != ClearFlags::None) {
        rt.clear(color);
    }
}

void RenderTarget::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
    if (!m_valid || !m_renderTexture) return;

    sf::RenderStates finalStates = states;
    if (!m_stateStack.empty()) {
        finalStates = m_stateStack.back();
    }
    finalStates.blendMode = m_blendMode;

    m_renderTexture->draw(drawable, finalStates);
}

void RenderTarget::draw(const sf::Vertex* vertices, std::size_t count, sf::PrimitiveType type,
                        const sf::RenderStates& states) {
    if (!m_valid || !m_renderTexture) return;

    sf::RenderStates finalStates = states;
    if (!m_stateStack.empty()) {
        finalStates = m_stateStack.back();
    }
    finalStates.blendMode = m_blendMode;

    m_renderTexture->draw(vertices, count, type, finalStates);
}

void RenderTarget::display() {
    if (m_valid && m_renderTexture) {
        m_renderTexture->display();
    }
}

void RenderTarget::setView(const sf::View& view) {
    if (m_valid && m_renderTexture) {
        m_currentView = view;
        m_renderTexture->setView(view);
    }
}

const sf::View& RenderTarget::getView() const {
    return m_currentView;
}

void RenderTarget::setDefaultView() {
    if (m_valid && m_renderTexture) {
        m_currentView = m_defaultView;
        m_renderTexture->setView(m_defaultView);
    }
}

sf::View RenderTarget::getDefaultView() const {
    return m_defaultView;
}

void RenderTarget::setActive(bool active) {
    if (m_valid && m_renderTexture) {
        m_renderTexture->setActive(active);
    }
}

sf::Vector2u RenderTarget::getSize() const {
    return m_valid ? m_desc.size : sf::Vector2u(0, 0);
}

sf::FloatRect RenderTarget::getVisibleArea() const {
    if (!m_valid) return sf::FloatRect();
    sf::Vector2f halfSize = m_currentView.getSize() * 0.5f;
    sf::Vector2f center = m_currentView.getCenter();
    return sf::FloatRect(center.x - halfSize.x, center.y - halfSize.y,
                         m_currentView.getSize().x, m_currentView.getSize().y);
}

TargetType RenderTarget::getType() const {
    return m_type;
}

sf::RenderTarget* RenderTarget::getNativeTarget() {
    return m_renderTexture.get();
}

const sf::RenderTarget* RenderTarget::getNativeTarget() const {
    return m_renderTexture.get();
}

sf::RenderTexture* RenderTarget::getRenderTexture() {
    return m_renderTexture.get();
}

const sf::RenderTexture* RenderTarget::getRenderTexture() const {
    return m_renderTexture.get();
}

const sf::Texture& RenderTarget::getTexture() const {
    static sf::Texture empty;
    return m_valid ? m_renderTexture->getTexture() : empty;
}

bool RenderTarget::isValid() const {
    return m_valid;
}

void RenderTarget::release() {
    m_renderTexture.reset();
    m_type = TargetType::Null;
    m_valid = false;
    m_stateStack.clear();
}

void RenderTarget::setSmooth(bool smooth) {
    m_desc.smooth = smooth;
    if (m_valid && m_renderTexture) {
        m_renderTexture->setSmooth(smooth);
    }
}

bool RenderTarget::isSmooth() const {
    return m_desc.smooth;
}

void RenderTarget::setRepeated(bool repeated) {
    m_desc.repeated = repeated;
    if (m_valid && m_renderTexture) {
        m_renderTexture->setRepeated(repeated);
    }
}

bool RenderTarget::isRepeated() const {
    return m_desc.repeated;
}

void RenderTarget::setName(const std::string& name) {
    m_desc.name = name;
}

std::string RenderTarget::getName() const {
    return m_desc.name;
}

RenderTargetDesc RenderTarget::getDescription() const {
    return m_desc;
}

bool RenderTarget::saveToFile(const std::string& filePath) {
    if (!m_valid || !m_renderTexture) return false;
    return m_renderTexture->getTexture().copyToImage().saveToFile(filePath);
}

void RenderTarget::pushStates(const sf::RenderStates& states) {
    m_stateStack.push_back(states);
}

void RenderTarget::popStates() {
    if (!m_stateStack.empty()) {
        m_stateStack.pop_back();
    }
}

void RenderTarget::setBlendMode(sf::BlendMode mode) {
    m_blendMode = mode;
}

sf::BlendMode RenderTarget::getBlendMode() const {
    return m_blendMode;
}

} // namespace graphics
} // namespace engine
