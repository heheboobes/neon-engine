#include <engine/core/Application.h>
#include <engine/ecs/World.h>
#include <engine/graphics/SpriteComponent.h>
#include <engine/physics/TransformComponent.h>
#include "PlayerController.h"
#include "LevelLoader.h"

int main()
{
    engine::Application app("Platformer", 1280, 720);
    engine::ecs::World& world = app.getWorld();

    auto level = LevelLoader::load("assets/levels/level1.json");
    for (auto& entity : level.entities)
    {
        world.addEntity(std::move(entity));
    }

    auto player = world.createEntity();
    player.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(64.f, 480.f), 0.f, sf::Vector2f(1.f, 1.f)
    );
    player.addComponent<engine::graphics::SpriteComponent>(
        "assets/textures/player_idle.png"
    );
    player.addComponent<PlayerComponent>(3, false, false);

    world.registerSystem<PlayerController>();
    world.registerSystem<LevelRenderer>();

    app.setFixedUpdate([&](float dt) {
        world.update(dt);
    });

    app.setRender([&](sf::RenderWindow& window) {
        world.render(window);
    });

    app.run();
    return 0;
}
