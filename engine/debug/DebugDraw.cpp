#include "DebugDraw.h"
#include <cmath>
#include <cstring>

namespace engine::debug {

DebugDraw& DebugDraw::instance() {
    static DebugDraw s_instance;
    return s_instance;
}

void DebugDraw::setTarget(sf::RenderTarget* target) {
    m_target = target;
}

sf::RenderTarget* DebugDraw::getTarget() const {
    return m_target;
}

void DebugDraw::setVisible(bool visible) {
    m_visible = visible;
}

bool DebugDraw::isVisible() const {
    return m_visible;
}

void DebugDraw::drawRect(const sf::FloatRect& rect, const sf::Color& color,
                         float thickness, float depth) {
    float t = thickness * 0.5f;
    sf::Vector2f p0(rect.left - t, rect.top - t);
    sf::Vector2f p1(rect.left + rect.width + t, rect.top - t);
    sf::Vector2f p2(rect.left + rect.width + t, rect.top + rect.height + t);
    sf::Vector2f p3(rect.left - t, rect.top + rect.height + t);

    drawLine(p0, p1, color, thickness);
    drawLine(p1, p2, color, thickness);
    drawLine(p2, p3, color, thickness);
    drawLine(p3, p0, color, thickness);
}

void DebugDraw::drawRectFilled(const sf::FloatRect& rect, const sf::Color& fillColor,
                               float depth) {
    sf::Vector2f p0(rect.left, rect.top);
    sf::Vector2f p1(rect.left + rect.width, rect.top);
    sf::Vector2f p2(rect.left + rect.width, rect.top + rect.height);
    sf::Vector2f p3(rect.left, rect.top + rect.height);

    m_triangleVertices.push_back(sf::Vertex(p0, fillColor));
    m_triangleVertices.push_back(sf::Vertex(p1, fillColor));
    m_triangleVertices.push_back(sf::Vertex(p2, fillColor));

    m_triangleVertices.push_back(sf::Vertex(p0, fillColor));
    m_triangleVertices.push_back(sf::Vertex(p2, fillColor));
    m_triangleVertices.push_back(sf::Vertex(p3, fillColor));
}

void DebugDraw::drawCircle(const sf::Vector2f& center, float radius,
                           const sf::Color& color, float thickness) {
    static const int kSegments = 32;
    for (int i = 0; i < kSegments; ++i) {
        float a1 = 2.0f * 3.14159265f * static_cast<float>(i) / kSegments;
        float a2 = 2.0f * 3.14159265f * static_cast<float>(i + 1) / kSegments;
        sf::Vector2f p1(center.x + radius * std::cos(a1),
                        center.y + radius * std::sin(a1));
        sf::Vector2f p2(center.x + radius * std::cos(a2),
                        center.y + radius * std::sin(a2));
        drawLine(p1, p2, color, thickness);
    }
}

void DebugDraw::drawCircleFilled(const sf::Vector2f& center, float radius,
                                 const sf::Color& fillColor) {
    static const int kSegments = 32;
    for (int i = 0; i < kSegments; ++i) {
        float a1 = 2.0f * 3.14159265f * static_cast<float>(i) / kSegments;
        float a2 = 2.0f * 3.14159265f * static_cast<float>(i + 1) / kSegments;
        sf::Vector2f p1(center.x + radius * std::cos(a1),
                        center.y + radius * std::sin(a1));
        sf::Vector2f p2(center.x + radius * std::cos(a2),
                        center.y + radius * std::sin(a2));

        m_triangleVertices.push_back(sf::Vertex(center, fillColor));
        m_triangleVertices.push_back(sf::Vertex(p1, fillColor));
        m_triangleVertices.push_back(sf::Vertex(p2, fillColor));
    }
}

void DebugDraw::drawLine(const sf::Vector2f& from, const sf::Vector2f& to,
                         const sf::Color& color, float thickness) {
    sf::Vector2f dir = to - from;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.0001f) return;

    sf::Vector2f perp(-dir.y / len * thickness * 0.5f,
                       dir.x / len * thickness * 0.5f);

    sf::Vector2f p0 = from + perp;
    sf::Vector2f p1 = from - perp;
    sf::Vector2f p2 = to - perp;
    sf::Vector2f p3 = to + perp;

    m_triangleVertices.push_back(sf::Vertex(p0, color));
    m_triangleVertices.push_back(sf::Vertex(p1, color));
    m_triangleVertices.push_back(sf::Vertex(p2, color));

    m_triangleVertices.push_back(sf::Vertex(p0, color));
    m_triangleVertices.push_back(sf::Vertex(p2, color));
    m_triangleVertices.push_back(sf::Vertex(p3, color));
}

void DebugDraw::drawText(const sf::Vector2f& position, const std::string& text,
                         const sf::Color& color, unsigned int charSize,
                         const sf::Font* font) {
    if (!font && !m_fontLoaded) {
        m_fontLoaded = m_defaultFont.loadFromFile("assets/fonts/consolas.ttf");
    }

    sf::Text sfText;
    sfText.setFont(font ? *font : m_defaultFont);
    sfText.setString(text);
    sfText.setCharacterSize(charSize);
    sfText.setFillColor(color);
    sfText.setPosition(position);
    m_texts.push_back(sfText);
}

void DebugDraw::drawGrid(const sf::FloatRect& area, float cellSize,
                         const sf::Color& color) {
    float left = std::floor(area.left / cellSize) * cellSize;
    float top = std::floor(area.top / cellSize) * cellSize;
    float right = area.left + area.width;
    float bottom = area.top + area.height;

    for (float x = left; x <= right; x += cellSize) {
        drawLine(sf::Vector2f(x, area.top), sf::Vector2f(x, bottom), color, 0.5f);
    }
    for (float y = top; y <= bottom; y += cellSize) {
        drawLine(sf::Vector2f(area.left, y), sf::Vector2f(right, y), color, 0.5f);
    }
}

void DebugDraw::flush() {
    if (!m_target || !m_visible) {
        clear();
        return;
    }

    if (!m_lineVertices.empty()) {
        m_target->draw(m_lineVertices.data(), m_lineVertices.size(),
                       sf::PrimitiveType::Lines);
    }

    if (!m_triangleVertices.empty()) {
        m_target->draw(m_triangleVertices.data(), m_triangleVertices.size(),
                       sf::PrimitiveType::Triangles);
    }

    for (auto& text : m_texts) {
        m_target->draw(text);
    }

    clear();
}

void DebugDraw::clear() {
    m_lineVertices.clear();
    m_triangleVertices.clear();
    m_texts.clear();
}

} // namespace engine::debug
