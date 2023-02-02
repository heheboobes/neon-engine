#include "PlayerController.h"
#include <engine/physics/TransformComponent.h>
#include <engine/graphics/SpriteComponent.h>
#include <SFML/Graphics.hpp>
#include <algorithm>

namespace examples::platformer
{

void PlayerController::update(float dt)
{
    for (auto& entity : getEntities())
    {
        handleInput(entity, dt);
        updateAnimation(entity, dt);
    }
}

void PlayerController::render(sf::RenderWindow& window)
{
    for (auto& entity : getEntities())
    {
        auto* sprite = entity.getComponent<engine::graphics::SpriteComponent>();
        auto* transform = entity.getComponent<engine::physics::TransformComponent>();
        if (sprite && transform)
        {
            sprite->sprite.setPosition(transform->position);
            sprite->sprite.setRotation(transform->rotation);
            sprite->sprite.setScale(transform->scale);
            window.draw(sprite->sprite);
        }
    }
}

void PlayerController::handleInput(engine::ecs::Entity& entity, float dt)
{
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    auto* player = entity.getComponent<PlayerComponent>();
    if (!transform || !player) return;

    sf::Vector2f velocity(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        velocity.x -= MOVE_SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        velocity.x += MOVE_SPEED;

    velocity.y = std::min(velocity.y + GRAVITY * dt, MAX_FALL_SPEED);
    transform->position += velocity * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player->isGrounded)
    {
        applyJump(entity, dt);
    }
}

void PlayerController::updateAnimation(engine::ecs::Entity& entity, float dt)
{
    auto* sprite = entity.getComponent<engine::graphics::SpriteComponent>();
    auto* player = entity.getComponent<PlayerComponent>();
    if (!sprite || !player) return;

    static float elapsed = 0.f;
    elapsed += dt;

    if (player->isAttacking)
    {
        sprite->textureRect = sf::IntRect(96, 0, 32, 48);
    }
    else if (!player->isGrounded)
    {
        sprite->textureRect = sf::IntRect(64, 0, 32, 48);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        int frame = static_cast<int>(elapsed / 0.12f) % 4;
        sprite->textureRect = sf::IntRect(frame * 32, 0, 32, 48);
    }
    else
    {
        sprite->textureRect = sf::IntRect(0, 0, 32, 48);
    }
}

void PlayerController::applyJump(engine::ecs::Entity& entity, float dt)
{
    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    auto* player = entity.getComponent<PlayerComponent>();
    if (!transform || !player) return;

    transform->position.y += JUMP_VELOCITY * dt;
    player->isGrounded = false;
}

}
