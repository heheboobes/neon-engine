#include "View.h"
#include <cassert>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace engine {
namespace graphics {

View::View()
    : m_center(0.f, 0.f)
    , m_size(800.f, 600.f)
    , m_rotation(0.f)
    , m_viewport(0.f, 0.f, 1.f, 1.f)
    , m_hasScissorRect(false)
    , m_inheritRotation(true)
    , m_dirty(true) {}

View::View(const sf::FloatRect& rect)
    : m_center(rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f)
    , m_size(rect.width, rect.height)
    , m_rotation(0.f)
    , m_viewport(0.f, 0.f, 1.f, 1.f)
    , m_hasScissorRect(false)
    , m_inheritRotation(true)
    , m_dirty(true) {}

View::View(const sf::Vector2f& center, const sf::Vector2f& size)
    : m_center(center)
    , m_size(size)
    , m_rotation(0.f)
    , m_viewport(0.f, 0.f, 1.f, 1.f)
    , m_hasScissorRect(false)
    , m_inheritRotation(true)
    , m_dirty(true) {}

View::View(const sf::View& view)
    : m_center(view.getCenter())
    , m_size(view.getSize())
    , m_rotation(view.getRotation())
    , m_viewport(view.getViewport())
    , m_hasScissorRect(false)
    , m_inheritRotation(true)
    , m_sfmlView(view)
    , m_dirty(false) {}

void View::setCenter(const sf::Vector2f& center) {
    m_center = center;
    m_dirty = true;
}

void View::setCenter(float x, float y) {
    m_center = { x, y };
    m_dirty = true;
}

sf::Vector2f View::getCenter() const {
    return m_center;
}

void View::setSize(const sf::Vector2f& size) {
    m_size = size;
    m_dirty = true;
}

void View::setSize(float width, float height) {
    m_size = { width, height };
    m_dirty = true;
}

sf::Vector2f View::getSize() const {
    return m_size;
}

void View::setRotation(float angle) {
    m_rotation = std::fmod(angle, 360.f);
    if (m_rotation < 0.f) m_rotation += 360.f;
    m_dirty = true;
}

void View::rotate(float angle) {
    m_rotation = std::fmod(m_rotation + angle, 360.f);
    if (m_rotation < 0.f) m_rotation += 360.f;
    m_dirty = true;
}

float View::getRotation() const {
    return m_rotation;
}

void View::setViewport(const sf::FloatRect& viewport) {
    m_viewport = viewport;
    m_dirty = true;
}

sf::FloatRect View::getViewport() const {
    return m_viewport;
}

void View::setScissorRect(const sf::FloatRect& rect) {
    m_scissorRect = rect;
    m_hasScissorRect = true;
}

void View::clearScissorRect() {
    m_hasScissorRect = false;
}

sf::FloatRect View::getScissorRect() const {
    return m_scissorRect;
}

bool View::hasScissorRect() const {
    return m_hasScissorRect;
}

void View::setLetterbox(float targetAspect) {
    float currentAspect = getAspectRatio();
    if (std::abs(currentAspect - targetAspect) < 0.0001f) return;

    if (currentAspect > targetAspect) {
        float newWidth = m_size.y * targetAspect;
        float offset = (m_size.x - newWidth) * 0.5f;
        m_viewport = sf::FloatRect(offset / m_size.x, 0.f, newWidth / m_size.x, 1.f);
    } else {
        float newHeight = m_size.x / targetAspect;
        float offset = (m_size.y - newHeight) * 0.5f;
        m_viewport = sf::FloatRect(0.f, offset / m_size.y, 1.f, newHeight / m_size.y);
    }
    m_dirty = true;
}

void View::fitToSize(const sf::Vector2u& targetSize) {
    m_size = sf::Vector2f(static_cast<float>(targetSize.x),
                          static_cast<float>(targetSize.y));
    m_viewport = sf::FloatRect(0.f, 0.f, 1.f, 1.f);
    m_dirty = true;
}

void View::zoom(float factor) {
    m_size *= factor;
    m_dirty = true;
}

void View::move(const sf::Vector2f& offset) {
    m_center += offset;
    m_dirty = true;
}

void View::move(float dx, float dy) {
    m_center += sf::Vector2f(dx, dy);
    m_dirty = true;
}

void View::reset(const sf::FloatRect& rect) {
    m_center = { rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f };
    m_size = { rect.width, rect.height };
    m_rotation = 0.f;
    m_viewport = { 0.f, 0.f, 1.f, 1.f };
    m_dirty = true;
}

void View::assign(const sf::View& view) {
    m_center = view.getCenter();
    m_size = view.getSize();
    m_rotation = view.getRotation();
    m_viewport = view.getViewport();
    m_sfmlView = view;
    m_dirty = false;
}

sf::View View::toSFML() const {
    updateSFML();
    return m_sfmlView;
}

View::operator sf::View() const {
    updateSFML();
    return m_sfmlView;
}

void View::applyTo(sf::RenderTarget& target) const {
    updateSFML();
    target.setView(m_sfmlView);
}

sf::FloatRect View::getBounds() const {
    float halfW = m_size.x * 0.5f;
    float halfH = m_size.y * 0.5f;

    if (std::abs(m_rotation) < 0.001f) {
        return sf::FloatRect(m_center.x - halfW, m_center.y - halfH, m_size.x, m_size.y);
    }

    float rad = m_rotation * 3.14159265f / 180.f;
    float cosA = std::abs(std::cos(rad));
    float sinA = std::abs(std::sin(rad));
    float rotW = m_size.x * cosA + m_size.y * sinA;
    float rotH = m_size.x * sinA + m_size.y * cosA;

    return sf::FloatRect(m_center.x - rotW * 0.5f, m_center.y - rotH * 0.5f, rotW, rotH);
}

bool View::contains(const sf::Vector2f& point) const {
    return getBounds().contains(point);
}

bool View::intersects(const sf::FloatRect& rect) const {
    return getBounds().intersects(rect);
}

sf::Vector2f View::toWorldCoords(const sf::RenderTarget& target, const sf::Vector2i& screenCoords) const {
    updateSFML();
    return target.mapPixelToCoords(screenCoords, m_sfmlView);
}

sf::Vector2i View::toScreenCoords(const sf::RenderTarget& target, const sf::Vector2f& worldCoords) const {
    updateSFML();
    return target.mapCoordsToPixel(worldCoords, m_sfmlView);
}

float View::getAspectRatio() const {
    return (m_size.y > 0.f) ? m_size.x / m_size.y : 1.f;
}

void View::setInheritRotation(bool inherit) {
    m_inheritRotation = inherit;
}

bool View::getInheritRotation() const {
    return m_inheritRotation;
}

void View::setShader(const std::string& shaderId) {
    m_shaderId = shaderId;
}

std::string View::getShader() const {
    return m_shaderId;
}

bool View::hasShader() const {
    return !m_shaderId.empty();
}

std::string View::toString() const {
    std::ostringstream oss;
    oss << "View(center=" << m_center.x << "," << m_center.y
        << " size=" << m_size.x << "x" << m_size.y
        << " rotation=" << m_rotation
        << " viewport=" << m_viewport.left << "," << m_viewport.top << ","
        << m_viewport.width << "x" << m_viewport.height << ")";
    return oss.str();
}

bool View::operator==(const View& other) const {
    return m_center == other.m_center &&
           m_size == other.m_size &&
           std::abs(m_rotation - other.m_rotation) < 0.001f &&
           m_viewport == other.m_viewport;
}

bool View::operator!=(const View& other) const {
    return !(*this == other);
}

View View::lerp(const View& from, const View& to, float t) {
    View result;
    result.m_center = from.m_center + (to.m_center - from.m_center) * t;
    result.m_size = from.m_size + (to.m_size - from.m_size) * t;
    result.m_rotation = from.m_rotation + (to.m_rotation - from.m_rotation) * t;
    result.m_viewport = from.m_viewport;
    return result;
}

void View::updateSFML() const {
    if (!m_dirty) return;
    m_sfmlView.setCenter(m_center);
    m_sfmlView.setSize(m_size);
    m_sfmlView.setRotation(m_rotation);
    m_sfmlView.setViewport(m_viewport);
    m_dirty = false;
}

} // namespace graphics
} // namespace engine

