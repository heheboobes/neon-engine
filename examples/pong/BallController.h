#pragma once

#include <engine/ecs/System.h>
#include <SFML/System/Vector2.hpp>

namespace examples::pong
{

struct BallComponent
{
    sf::Vector2f velocity;
    float speed;
    bool isStuck;
    int hitCount;
};

class BallController : public engine::ecs::System
{
public:
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    int getLeftScore() const { return m_leftScore; }
    int getRightScore() const { return m_rightScore; }

private:
    void handleWallCollision(engine::ecs::Entity& entity, float dt);
    void handlePaddleCollision(engine::ecs::Entity& entity);
    void resetBall(engine::ecs::Entity& entity);
    void checkScoring(engine::ecs::Entity& entity);

    sf::Vector2f getPaddleBounds(engine::ecs::Entity& paddle);
    bool rectOverlap(const sf::Vector2f& aPos, const sf::Vector2f& aSize,
                     const sf::Vector2f& bPos, const sf::Vector2f& bSize);

    int m_leftScore = 0;
    int m_rightScore = 0;

    static constexpr float BALL_RADIUS = 8.f;
    static constexpr float INITIAL_SPEED = 350.f;
    static constexpr float MAX_SPEED = 700.f;
    static constexpr float SPEED_INCREMENT = 20.f;
    static constexpr int WIN_SCORE = 7;
};

}
