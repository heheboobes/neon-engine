#include "GameUI.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <algorithm>
#include <sstream>

namespace examples::rpg
{

GameUI::GameUI()
    : m_healthCurrent(100.f)
    , m_healthMax(100.f)
    , m_dialogVisible(false)
    , m_animTimer(0.f)
{
    if (!m_font.loadFromFile("assets/fonts/ui_font.ttf"))
    {
        sf::Font defaultFont;
        m_font = defaultFont;
    }
    buildCanvas();
}

void GameUI::buildCanvas()
{
    m_widgets.clear();

    UIWidget healthFrame;
    healthFrame.bounds = sf::FloatRect(20.f, 20.f, 220.f, 30.f);
    healthFrame.backgroundColor = sf::Color(50, 50, 50, 200);
    healthFrame.label = "HP";
    healthFrame.visible = true;
    healthFrame.interactable = false;
    m_widgets.push_back(healthFrame);

    UIWidget invPanel;
    invPanel.bounds = sf::FloatRect(780.f, 20.f, 220.f, 400.f);
    invPanel.backgroundColor = sf::Color(30, 30, 50, 220);
    invPanel.label = "Inventory";
    invPanel.visible = true;
    invPanel.interactable = true;
    m_widgets.push_back(invPanel);

    UIWidget menuBtn;
    menuBtn.bounds = sf::FloatRect(20.f, 700.f, 120.f, 40.f);
    menuBtn.backgroundColor = sf::Color(80, 40, 40, 200);
    menuBtn.label = "Menu";
    menuBtn.visible = true;
    menuBtn.interactable = true;
    m_widgets.push_back(menuBtn);
}

void GameUI::update(float dt)
{
    m_animTimer += dt;
    if (m_animTimer > 2.f) m_animTimer = 0.f;

    for (auto& widget : m_widgets)
    {
        if (!widget.visible) continue;
        if (widget.label == "HP")
        {
            float ratio = m_healthCurrent / std::max(m_healthMax, 0.01f);
            widget.backgroundColor = sf::Color(
                static_cast<sf::Uint8>(255 * (1.f - ratio)),
                static_cast<sf::Uint8>(255 * ratio),
                40, 200
            );
        }
    }
}

void GameUI::render(sf::RenderWindow& window)
{
    for (auto& widget : m_widgets)
    {
        if (!widget.visible) continue;
        sf::RectangleShape bg(sf::Vector2f(widget.bounds.width, widget.bounds.height));
        bg.setPosition(widget.bounds.left, widget.bounds.top);
        bg.setFillColor(widget.backgroundColor);
        window.draw(bg);

        sf::Text label(widget.label, m_font, 16);
        label.setFillColor(widget.textColor);
        label.setPosition(widget.bounds.left + 8.f, widget.bounds.top + 6.f);
        window.draw(label);
    }

    renderHealthBar(window);
    renderInventoryPanel(window);
    renderDialogBox(window);
}

void GameUI::renderHealthBar(sf::RenderWindow& window)
{
    float barWidth = 180.f;
    float barHeight = 20.f;
    float x = 30.f, y = 26.f;
    float ratio = m_healthCurrent / std::max(m_healthMax, 0.01f);

    sf::RectangleShape background(sf::Vector2f(barWidth, barHeight));
    background.setPosition(x, y);
    background.setFillColor(sf::Color(60, 20, 20));
    window.draw(background);

    sf::RectangleShape fill(sf::Vector2f(barWidth * ratio, barHeight));
    fill.setPosition(x, y);
    fill.setFillColor(sf::Color(220, 40, 40));
    window.draw(fill);

    std::stringstream ss;
    ss << static_cast<int>(m_healthCurrent) << " / " << static_cast<int>(m_healthMax);
    sf::Text hpText(ss.str(), m_font, 14);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(x + barWidth / 2.f - 30.f, y + 2.f);
    window.draw(hpText);
}

void GameUI::renderInventoryPanel(sf::RenderWindow& window)
{
    float x = 790.f, y = 50.f;
    sf::Text invTitle("Inventory", m_font, 18);
    invTitle.setFillColor(sf::Color::Yellow);
    invTitle.setPosition(x, y);
    window.draw(invTitle);

    y += 30.f;
    for (size_t i = 0; i < m_inventoryItems.size() && i < 8; ++i)
    {
        std::stringstream ss;
        ss << m_inventoryItems[i].first << " x" << m_inventoryItems[i].second;
        sf::Text itemText(ss.str(), m_font, 14);
        itemText.setFillColor(sf::Color::White);
        itemText.setPosition(x + 10.f, y);
        window.draw(itemText);
        y += 24.f;
    }
}

void GameUI::renderDialogBox(sf::RenderWindow& window)
{
    if (!m_dialogVisible) return;

    sf::RectangleShape dialogBg(sf::Vector2f(800.f, 140.f));
    dialogBg.setPosition(112.f, 560.f);
    dialogBg.setFillColor(sf::Color(10, 10, 20, 230));
    dialogBg.setOutlineThickness(2.f);
    dialogBg.setOutlineColor(sf::Color::White);
    window.draw(dialogBg);

    sf::Text speaker(m_dialogSpeaker + ":", m_font, 18);
    speaker.setFillColor(sf::Color::Cyan);
    speaker.setPosition(130.f, 575.f);
    window.draw(speaker);

    int visibleChars = static_cast<int>(m_animTimer * 60.f);
    std::string shown = m_dialogText.substr(0, std::min<size_t>(visibleChars, m_dialogText.size()));
    sf::Text dialog(shown, m_font, 16);
    dialog.setFillColor(sf::Color::White);
    dialog.setPosition(130.f, 605.f);
    window.draw(dialog);
}

void GameUI::renderMenuBar(sf::RenderWindow& window)
{
    float y = 700.f;
    sf::Text menuText("[ESC] Pause  [I] Inventory  [M] Map", m_font, 14);
    menuText.setFillColor(sf::Color(180, 180, 180));
    menuText.setPosition(20.f, y);
    window.draw(menuText);
}

void GameUI::showDialog(const std::string& speaker, const std::string& text)
{
    m_dialogVisible = true;
    m_dialogSpeaker = speaker;
    m_dialogText = text;
    m_animTimer = 0.f;
}

void GameUI::hideDialog()
{
    m_dialogVisible = false;
    m_dialogSpeaker.clear();
    m_dialogText.clear();
}

void GameUI::updateHealthBar(float current, float max)
{
    m_healthCurrent = std::max(0.f, current);
    m_healthMax = std::max(1.f, max);
}

void GameUI::addInventoryItem(const std::string& name, int count)
{
    for (auto& item : m_inventoryItems)
    {
        if (item.first == name)
        {
            item.second += count;
            return;
        }
    }
    m_inventoryItems.emplace_back(name, count);
}

}
