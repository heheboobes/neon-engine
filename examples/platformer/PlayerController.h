#pragma once

#include <engine/ecs/System.h>
#include <engine/ecs/Entity.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>

namespace examples::platformer
{

struct PlayerComponent
{
    int health;
    bool isGrounded;
    bool isAttacking;
};

class PlayerController : public engine::ecs::System
{
public:
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void handleInput(engine::ecs::Entity& entity, float dt);
    void updateAnimation(engine::ecs::Entity& entity, float dt);
    void applyJump(engine::ecs::Entity& entity, float dt);

    static constexpr float MOVE_SPEED = 280.f;
    static constexpr float JUMP_VELOCITY = -520.f;
    static constexpr float GRAVITY = 980.f;
    static constexpr float MAX_FALL_SPEED = 600.f;
};

}
