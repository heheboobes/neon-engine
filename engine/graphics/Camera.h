#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace engine {
namespace graphics {

class Camera {
public:
    Camera();
    explicit Camera(const sf::FloatRect& rect);
    explicit Camera(const sf::Vector2f& center, const sf::Vector2f& size);
    ~Camera() = default;

    void setCenter(const sf::Vector2f& center);
    void setCenter(float x, float y);
    sf::Vector2f getCenter() const;

    void setSize(const sf::Vector2f& size);
    void setSize(float width, float height);
    sf::Vector2f getSize() const;

    void move(const sf::Vector2f& offset);
    void move(float dx, float dy);

    void setZoom(float factor);
    void zoom(float factor);
    float getZoom() const;

    void setRotation(float angle);
    void rotate(float angle);
    float getRotation() const;

    void setViewport(const sf::FloatRect& viewport);
    sf::FloatRect getViewport() const;

    void lookAt(const sf::Vector2f& target);
    void lookAt(float x, float y);

    void shake(float intensity, float duration);
    void update(float dt);
    bool isShaking() const;
    float getShakeIntensity() const;

    void setBounds(const sf::FloatRect& bounds);
    void clearBounds();
    sf::FloatRect getBounds() const;
    bool hasBounds() const;

    void constrain();
    sf::FloatRect getVisibleArea() const;

    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;

    sf::View getView() const;
    void applyTo(sf::RenderTarget& target) const;
    void applyTo(sf::View& view) const;

    void reset(const sf::FloatRect& rect);
    Camera lerpTo(const Camera& target, float t) const;

    std::string toString() const;

private:
    sf::Vector2f m_center;
    sf::Vector2f m_size;
    float m_rotation;
    float m_zoom;
    sf::FloatRect m_viewport;

    sf::FloatRect m_bounds;
    bool m_hasBounds;

    bool m_shaking;
    float m_shakeIntensity;
    float m_shakeDuration;
    float m_shakeElapsed;
    sf::Vector2f m_shakeOffset;
};

} // namespace graphics
} // namespace engine
