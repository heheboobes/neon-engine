#include "Camera.h"
#include <cassert>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace engine {
namespace graphics {

Camera::Camera()
    : m_center(0.f, 0.f)
    , m_size(800.f, 600.f)
    , m_rotation(0.f)
    , m_zoom(1.f)
    , m_viewport(0.f, 0.f, 1.f, 1.f)
    , m_hasBounds(false)
    , m_shaking(false)
    , m_shakeIntensity(0.f)
    , m_shakeDuration(0.f)
    , m_shakeElapsed(0.f) {}

Camera::Camera(const sf::FloatRect& rect)
    : m_center(rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f)
    , m_size(rect.width, rect.height)
    , m_rotation(0.f)
    , m_zoom(1.f)
    , m_viewport(0.f, 0.f, 1.f, 1.f)
    , m_hasBounds(false)
    , m_shaking(false)
    , m_shakeIntensity(0.f)
    , m_shakeDuration(0.f)
    , m_shakeElapsed(0.f) {}

Camera::Camera(const sf::Vector2f& center, const sf::Vector2f& size)
    : m_center(center)
    , m_size(size)
    , m_rotation(0.f)
    , m_zoom(1.f)
    , m_viewport(0.f, 0.f, 1.f, 1.f)
    , m_hasBounds(false)
    , m_shaking(false)
    , m_shakeIntensity(0.f)
    , m_shakeDuration(0.f)
    , m_shakeElapsed(0.f) {}

void Camera::setCenter(const sf::Vector2f& center) {
    m_center = center;
    constrain();
}

void Camera::setCenter(float x, float y) {
    m_center = { x, y };
    constrain();
}

sf::Vector2f Camera::getCenter() const {
    return m_center;
}

void Camera::setSize(const sf::Vector2f& size) {
    m_size = size;
    constrain();
}

void Camera::setSize(float width, float height) {
    m_size = { width, height };
    constrain();
}

sf::Vector2f Camera::getSize() const {
    return m_size;
}

void Camera::move(const sf::Vector2f& offset) {
    m_center += offset;
    constrain();
}

void Camera::move(float dx, float dy) {
    m_center += sf::Vector2f(dx, dy);
    constrain();
}

void Camera::setZoom(float factor) {
    m_zoom = std::max(0.001f, factor);
}

void Camera::zoom(float factor) {
    m_zoom = std::max(0.001f, m_zoom * factor);
}

float Camera::getZoom() const {
    return m_zoom;
}

void Camera::setRotation(float angle) {
    m_rotation = std::fmod(angle, 360.f);
    if (m_rotation < 0.f) m_rotation += 360.f;
}

void Camera::rotate(float angle) {
    m_rotation = std::fmod(m_rotation + angle, 360.f);
    if (m_rotation < 0.f) m_rotation += 360.f;
}

float Camera::getRotation() const {
    return m_rotation;
}

void Camera::setViewport(const sf::FloatRect& viewport) {
    m_viewport = viewport;
}

sf::FloatRect Camera::getViewport() const {
    return m_viewport;
}

void Camera::lookAt(const sf::Vector2f& target) {
    m_center = target;
    constrain();
}

void Camera::lookAt(float x, float y) {
    m_center = { x, y };
    constrain();
}

void Camera::shake(float intensity, float duration) {
    m_shaking = true;
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
    m_shakeElapsed = 0.f;
}

void Camera::update(float dt) {
    if (!m_shaking) return;

    m_shakeElapsed += dt;
    if (m_shakeElapsed >= m_shakeDuration) {
        m_shaking = false;
        m_shakeOffset = { 0.f, 0.f };
        return;
    }

    float progress = m_shakeElapsed / m_shakeDuration;
    float decay = 1.f - progress;
    float angle = static_cast<float>(std::rand()) / RAND_MAX * 6.2831853f;
    float magnitude = m_shakeIntensity * decay;

    m_shakeOffset.x = std::cos(angle) * magnitude;
    m_shakeOffset.y = std::sin(angle) * magnitude;
}

bool Camera::isShaking() const {
    return m_shaking;
}

float Camera::getShakeIntensity() const {
    return m_shakeIntensity;
}

void Camera::setBounds(const sf::FloatRect& bounds) {
    m_bounds = bounds;
    m_hasBounds = true;
    constrain();
}

void Camera::clearBounds() {
    m_hasBounds = false;
}

sf::FloatRect Camera::getBounds() const {
    return m_bounds;
}

bool Camera::hasBounds() const {
    return m_hasBounds;
}

void Camera::constrain() {
    if (!m_hasBounds) return;

    float halfW = (m_size.x * m_zoom) * 0.5f;
    float halfH = (m_size.y * m_zoom) * 0.5f;

    float minX = m_bounds.left + halfW;
    float maxX = m_bounds.left + m_bounds.width - halfW;
    float minY = m_bounds.top + halfH;
    float maxY = m_bounds.top + m_bounds.height - halfH;

    if (m_bounds.width < m_size.x * m_zoom) {
        minX = maxX = m_bounds.left + m_bounds.width * 0.5f;
    }
    if (m_bounds.height < m_size.y * m_zoom) {
        minY = maxY = m_bounds.top + m_bounds.height * 0.5f;
    }

    m_center.x = std::clamp(m_center.x, minX, maxX);
    m_center.y = std::clamp(m_center.y, minY, maxY);
}

sf::FloatRect Camera::getVisibleArea() const {
    float halfW = (m_size.x * m_zoom) * 0.5f;
    float halfH = (m_size.y * m_zoom) * 0.5f;
    return sf::FloatRect(
        m_center.x - halfW + m_shakeOffset.x,
        m_center.y - halfH + m_shakeOffset.y,
        m_size.x * m_zoom,
        m_size.y * m_zoom
    );
}

void Camera::setPosition(const sf::Vector2f& position) {
    m_center = position;
    constrain();
}

sf::Vector2f Camera::getPosition() const {
    return m_center;
}

sf::View Camera::getView() const {
    sf::Vector2f shakeAdjusted = m_center + m_shakeOffset;
    sf::View view(shakeAdjusted, m_size * m_zoom);
    view.setRotation(m_rotation);
    view.setViewport(m_viewport);
    return view;
}

void Camera::applyTo(sf::RenderTarget& target) const {
    target.setView(getView());
}

void Camera::applyTo(sf::View& view) const {
    sf::Vector2f shakeAdjusted = m_center + m_shakeOffset;
    view.setCenter(shakeAdjusted);
    view.setSize(m_size * m_zoom);
    view.setRotation(m_rotation);
    view.setViewport(m_viewport);
}

void Camera::reset(const sf::FloatRect& rect) {
    m_center = { rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f };
    m_size = { rect.width, rect.height };
    m_rotation = 0.f;
    m_zoom = 1.f;
    m_shaking = false;
    m_shakeOffset = { 0.f, 0.f };
}

Camera Camera::lerpTo(const Camera& target, float t) const {
    Camera result;
    result.m_center = m_center + (target.m_center - m_center) * t;
    result.m_size = m_size + (target.m_size - m_size) * t;
    result.m_zoom = m_zoom + (target.m_zoom - m_zoom) * t;
    result.m_rotation = m_rotation + (target.m_rotation - m_rotation) * t;
    result.m_viewport = m_viewport;
    result.m_hasBounds = m_hasBounds || target.m_hasBounds;
    result.m_bounds = m_hasBounds ? m_bounds : target.m_bounds;
    result.m_shaking = false;
    return result;
}

std::string Camera::toString() const {
    std::ostringstream oss;
    oss << "Camera(center=" << m_center.x << "," << m_center.y
        << " size=" << m_size.x << "x" << m_size.y
        << " zoom=" << m_zoom
        << " rotation=" << m_rotation << ")";
    return oss.str();
}

} // namespace graphics
} // namespace engine

