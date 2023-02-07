#include <engine/core/Application.h>
#include <engine/ecs/World.h>
#include <engine/graphics/TextComponent.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "GameUI.h"
#include "InventorySystem.h"

enum class GameState
{
    Menu,
    Playing,
    Paused,
    Dialog,
    GameOver
};

class RpgApplication
{
public:
    RpgApplication()
        : m_app("RPG Quest", 1024, 768)
        , m_state(GameState::Menu)
    {
        m_world = &m_app.getWorld();
        m_world->registerSystem<examples::rpg::GameUI>();
        m_world->registerSystem<examples::rpg::InventorySystem>();

        if (!m_bgMusic.openFromFile("assets/audio/overworld.ogg"))
            std::cerr << "Warning: Could not load background music\n";

        m_app.setUpdate([this](float dt) { update(dt); });
        m_app.setRender([this](sf::RenderWindow& win) { render(win); });
    }

    void run() { m_app.run(); }

private:
    void update(float dt)
    {
        if (m_state == GameState::Playing)
        {
            m_world->update(dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                m_state = GameState::Paused;
        }
        else if (m_state == GameState::Menu)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                m_state = GameState::Playing;
                m_bgMusic.setLoop(true);
                m_bgMusic.play();
            }
        }
        else if (m_state == GameState::Paused)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                m_state = GameState::Playing;
        }
    }

    void render(sf::RenderWindow& window)
    {
        window.clear(sf::Color(20, 20, 40));
        if (m_state == GameState::Menu)
        {
            sf::Text title("RPG Quest\n\nPress Enter to Start", m_font, 48);
            title.setFillColor(sf::Color::White);
            title.setPosition(260.f, 200.f);
            window.draw(title);
        }
        else if (m_state == GameState::Playing || m_state == GameState::Paused)
        {
            m_world->render(window);
        }
        else if (m_state == GameState::GameOver)
        {
            sf::Text go("Game Over", m_font, 64);
            go.setFillColor(sf::Color::Red);
            go.setPosition(320.f, 300.f);
            window.draw(go);
        }
    }

    engine::Application m_app;
    engine::ecs::World* m_world;
    GameState m_state;
    sf::Music m_bgMusic;
    sf::Font m_font;
};

int main()
{
    RpgApplication game;
    game.run();
    return 0;
}
