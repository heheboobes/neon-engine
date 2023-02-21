#include "BallController.h"
#include "PaddleController.h"
#include <engine/physics/TransformComponent.h>
#include <engine/graphics/SpriteComponent.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace examples::pong
{

void BallController::update(float dt)
{
    for (auto& entity : getEntities())
    {
        auto* ball = entity.getComponent<BallComponent>();
        auto* transform = entity.getComponent<engine::physics::TransformComponent>();
        if (!ball || !transform) continue;

        if (ball->isStuck)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                ball->isStuck = false;
                float angle = (std::rand() % 60 - 30) * 3.14159f / 180.f;
                float dir = (std::rand() % 2 == 0) ? 1.f : -1.f;
                ball->velocity = sf::Vector2f(dir * std::cos(angle), std::sin(angle));
                ball->velocity *= ball->speed;
            }
            continue;
        }

        transform->position += ball->velocity * dt;
        handleWallCollision(entity, dt);
        handlePaddleCollision(entity);
        checkScoring(entity);
    }
}

void BallController::render(sf::RenderWindow& window)
{
    for (auto& entity : getEntities())
    {
        auto* transform = entity.getComponent<engine::physics::TransformComponent>();
        auto* sprite = entity.getComponent<engine::graphics::SpriteComponent>();
        if (!transform || !sprite) continue;

        sf::CircleShape circle(BALL_RADIUS);
        circle.setPosition(transform->position);
        circle.setFillColor(sprite->color);
        window.draw(circle);
    }
}

void BallController::handleWallCollision(engine::ecs::Entity& entity, float dt)
{
    auto* ball = entity.getComponent<BallComponent>();
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    if (!ball || !transform) return;

    if (transform->position.y <= 0.f)
    {
        transform->position.y = 0.f;
        ball->velocity.y = std::abs(ball->velocity.y);
    }
    if (transform->position.y >= 584.f)
    {
        transform->position.y = 584.f;
        ball->velocity.y = -std::abs(ball->velocity.y);
    }
}

void BallController::handlePaddleCollision(engine::ecs::Entity& entity)
{
    auto* ball = entity.getComponent<BallComponent>();
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    if (!ball || !transform) return;

    for (auto& other : getEntities())
    {
        auto* paddle = other.getComponent<PaddleComponent>();
        if (!paddle) continue;

        auto* paddleTransform = other.getComponent<engine::physics::TransformComponent>();
        auto* paddleSprite = other.getComponent<engine::graphics::SpriteComponent>();
        if (!paddleTransform || !paddleSprite) continue;

        sf::Vector2f ballSize(BALL_RADIUS * 2.f, BALL_RADIUS * 2.f);
        if (rectOverlap(transform->position, ballSize,
                        paddleTransform->position, paddleSprite->size))
        {
            float paddleCenter = paddleTransform->position.y + paddleSprite->size.y / 2.f;
            float hitOffset = (transform->position.y + BALL_RADIUS - paddleCenter) /
                              (paddleSprite->size.y / 2.f);
            hitOffset = std::max(-1.f, std::min(1.f, hitOffset));

            float angle = hitOffset * 60.f * 3.14159f / 180.f;
            float dir = (transform->position.x < paddleTransform->position.x) ? -1.f : 1.f;
            ball->velocity = sf::Vector2f(dir * std::cos(angle), std::sin(angle));
            ball->speed = std::min(ball->speed + SPEED_INCREMENT, MAX_SPEED);
            ball->velocity *= ball->speed;
            ball->hitCount++;

            transform->position.x = paddleTransform->position.x +
                ((dir > 0.f) ? paddleSprite->size.x + 1.f : -BALL_RADIUS * 2.f - 1.f);
        }
    }
}

void BallController::checkScoring(engine::ecs::Entity& entity)
{
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    if (!transform) return;

    if (transform->position.x < -20.f)
    {
        m_rightScore++;
        resetBall(entity);
    }
    else if (transform->position.x > 820.f)
    {
        m_leftScore++;
        resetBall(entity);
    }
}

bool BallController::rectOverlap(const sf::Vector2f& aPos, const sf::Vector2f& aSize,
                                  const sf::Vector2f& bPos, const sf::Vector2f& bSize)
{
    return (aPos.x < bPos.x + bSize.x &&
            aPos.x + aSize.x > bPos.x &&
            aPos.y < bPos.y + bSize.y &&
            aPos.y + aSize.y > bPos.y);
}

void BallController::resetBall(engine::ecs::Entity& entity)
{
    auto* ball = entity.getComponent<BallComponent>();
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    if (!ball || !transform) return;

    transform->position = sf::Vector2f(392.f, 292.f);
    ball->velocity = sf::Vector2f(0.f, 0.f);
    ball->speed = INITIAL_SPEED;
    ball->isStuck = true;
    ball->hitCount = 0;
}

}
