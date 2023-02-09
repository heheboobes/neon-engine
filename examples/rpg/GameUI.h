#pragma once

#include <engine/ecs/System.h>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace examples::rpg
{

struct UIWidget
{
    sf::FloatRect bounds;
    std::string label;
    sf::Color backgroundColor;
    sf::Color textColor;
    bool visible;
    bool interactable;
};

class GameUI : public engine::ecs::System
{
public:
    GameUI();
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void showDialog(const std::string& speaker, const std::string& text);
    void hideDialog();
    void updateHealthBar(float current, float max);
    void addInventoryItem(const std::string& name, int count);

private:
    void buildCanvas();
    void renderHealthBar(sf::RenderWindow& window);
    void renderInventoryPanel(sf::RenderWindow& window);
    void renderDialogBox(sf::RenderWindow& window);
    void renderMenuBar(sf::RenderWindow& window);

    sf::Font m_font;
    sf::Texture m_uiTexture;
    std::vector<UIWidget> m_widgets;

    float m_healthCurrent;
    float m_healthMax;
    bool m_dialogVisible;
    std::string m_dialogSpeaker;
    std::string m_dialogText;
    std::vector<std::pair<std::string, int>> m_inventoryItems;
    float m_animTimer;
};

}
