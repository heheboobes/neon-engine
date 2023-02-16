#include "PaddleController.h"
#include "BallController.h"
#include <engine/physics/TransformComponent.h>
#include <engine/graphics/SpriteComponent.h>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>

namespace examples::pong
{

void PaddleController::update(float dt)
{
    for (auto& entity : getEntities())
    {
        auto* paddle = entity.getComponent<PaddleComponent>();
        if (!paddle) continue;

        if (paddle->isAI)
        {
            paddle->aiTimer += dt;
            handleAI(entity, dt);
        }
        else
        {
            handlePlayerInput(entity, dt);
        }
    }
}

void PaddleController::render(sf::RenderWindow& window)
{
    for (auto& entity : getEntities())
    {
        auto* transform = entity.getComponent<engine::physics::TransformComponent>();
        auto* sprite = entity.getComponent<engine::graphics::SpriteComponent>();
        if (!transform || !sprite) continue;

        sf::RectangleShape shape(sprite->size);
        shape.setPosition(transform->position);
        shape.setFillColor(sprite->color);
        window.draw(shape);
    }
}

void PaddleController::handlePlayerInput(engine::ecs::Entity& entity, float dt)
{
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    auto* paddle = entity.getComponent<PaddleComponent>();
    if (!transform || !paddle) return;

    if (sf::Keyboard::isKeyPressed(paddle->upKey))
        transform->position.y -= PADDLE_SPEED * dt;
    if (sf::Keyboard::isKeyPressed(paddle->downKey))
        transform->position.y += PADDLE_SPEED * dt;

    clampPaddle(transform->position);
}

void PaddleController::handleAI(engine::ecs::Entity& entity, float dt)
{
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    auto* paddle = entity.getComponent<PaddleComponent>();
    if (!transform || !paddle) return;

    if (paddle->aiTimer < paddle->aiReactionTime) return;

    float ballY = m_ballPos.y;
    float paddleCenter = transform->position.y + 50.f;
    float diff = ballY - paddleCenter;

    if (std::abs(diff) > 10.f)
    {
        float direction = (diff > 0.f) ? 1.f : -1.f;
        transform->position.y += direction * AI_SPEED * dt;
    }

    paddle->aiTimer = 0.f;
    paddle->aiReactionTime = 0.05f + static_cast<float>(std::rand() % 50) / 1000.f;
    clampPaddle(transform->position);
}

void PaddleController::clampPaddle(sf::Vector2f& position)
{
    position.y = std::max(MIN_Y, std::min(MAX_Y, position.y));
}

}
