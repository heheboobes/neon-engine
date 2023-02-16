#pragma once

#include <engine/ecs/System.h>
#include <SFML/Window/Keyboard.hpp>

namespace examples::pong
{

struct PaddleComponent
{
    sf::Keyboard::Key upKey;
    sf::Keyboard::Key downKey;
    bool isAI;
    float aiReactionTime;
    float aiTimer;
    int score;
};

class PaddleController : public engine::ecs::System
{
public:
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void setBallPosition(sf::Vector2f pos) { m_ballPos = pos; }

private:
    void handlePlayerInput(engine::ecs::Entity& entity, float dt);
    void handleAI(engine::ecs::Entity& entity, float dt);
    void clampPaddle(sf::Vector2f& position);

    sf::Vector2f m_ballPos;
    static constexpr float PADDLE_SPEED = 420.f;
    static constexpr float AI_SPEED = 350.f;
    static constexpr float MAX_Y = 500.f;
    static constexpr float MIN_Y = 0.f;
};

}
