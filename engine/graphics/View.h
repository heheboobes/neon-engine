#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace engine {
namespace graphics {

class View {
public:
    View();
    explicit View(const sf::FloatRect& rect);
    explicit View(const sf::Vector2f& center, const sf::Vector2f& size);
    explicit View(const sf::View& view);
    ~View() = default;

    void setCenter(const sf::Vector2f& center);
    void setCenter(float x, float y);
    sf::Vector2f getCenter() const;

    void setSize(const sf::Vector2f& size);
    void setSize(float width, float height);
    sf::Vector2f getSize() const;

    void setRotation(float angle);
    void rotate(float angle);
    float getRotation() const;

    void setViewport(const sf::FloatRect& viewport);
    sf::FloatRect getViewport() const;

    void setScissorRect(const sf::FloatRect& rect);
    void clearScissorRect();
    sf::FloatRect getScissorRect() const;
    bool hasScissorRect() const;

    void setLetterbox(float targetAspect);
    void fitToSize(const sf::Vector2u& targetSize);

    void zoom(float factor);
    void move(const sf::Vector2f& offset);
    void move(float dx, float dy);

    void reset(const sf::FloatRect& rect);
    void assign(const sf::View& view);

    sf::View toSFML() const;
    operator sf::View() const;

    void applyTo(sf::RenderTarget& target) const;

    sf::FloatRect getBounds() const;
    bool contains(const sf::Vector2f& point) const;
    bool intersects(const sf::FloatRect& rect) const;
    sf::Vector2f toWorldCoords(const sf::RenderTarget& target, const sf::Vector2i& screenCoords) const;
    sf::Vector2i toScreenCoords(const sf::RenderTarget& target, const sf::Vector2f& worldCoords) const;

    float getAspectRatio() const;

    void setInheritRotation(bool inherit);
    bool getInheritRotation() const;

    void setShader(const std::string& shaderId);
    std::string getShader() const;
    bool hasShader() const;

    std::string toString() const;

    bool operator==(const View& other) const;
    bool operator!=(const View& other) const;

    static View lerp(const View& from, const View& to, float t);

private:
    void updateSFML() const;

    sf::Vector2f m_center;
    sf::Vector2f m_size;
    float m_rotation;
    sf::FloatRect m_viewport;

    sf::FloatRect m_scissorRect;
    bool m_hasScissorRect;

    bool m_inheritRotation;
    std::string m_shaderId;

    mutable sf::View m_sfmlView;
    mutable bool m_dirty;
};

} // namespace graphics
} // namespace engine
