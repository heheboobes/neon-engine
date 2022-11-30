#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace engine::debug {

struct DebugVertex {
    sf::Vector2f position;
    sf::Color color;
};

struct DebugLine {
    DebugVertex v0;
    DebugVertex v1;
};

class DebugDraw {
public:
    static DebugDraw& instance();

    void setTarget(sf::RenderTarget* target);
    sf::RenderTarget* getTarget() const;

    void drawRect(const sf::FloatRect& rect, const sf::Color& color,
                  float thickness = 1.0f, float depth = 0.0f);
    void drawRectFilled(const sf::FloatRect& rect, const sf::Color& fillColor,
                        float depth = 0.0f);
    void drawCircle(const sf::Vector2f& center, float radius,
                    const sf::Color& color, float thickness = 1.0f);
    void drawCircleFilled(const sf::Vector2f& center, float radius,
                          const sf::Color& fillColor);
    void drawLine(const sf::Vector2f& from, const sf::Vector2f& to,
                  const sf::Color& color, float thickness = 1.0f);
    void drawText(const sf::Vector2f& position, const std::string& text,
                  const sf::Color& color, unsigned int charSize = 14,
                  const sf::Font* font = nullptr);
    void drawGrid(const sf::FloatRect& area, float cellSize,
                  const sf::Color& color = sf::Color(80, 80, 80));

    void flush();
    void clear();

    void setVisible(bool visible);
    bool isVisible() const;

private:
    DebugDraw() = default;

    sf::RenderTarget* m_target{nullptr};
    bool m_visible{true};
    sf::Font m_defaultFont;
    bool m_fontLoaded{false};

    std::vector<sf::Vertex> m_lineVertices;
    std::vector<sf::Vertex> m_triangleVertices;
    std::vector<sf::Text> m_texts;

    static constexpr size_t kMaxVertices = 65536;
};

} // namespace engine::debug
