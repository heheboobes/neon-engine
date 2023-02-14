#include <engine/core/Application.h>
#include <engine/ecs/World.h>
#include <engine/graphics/SpriteComponent.h>
#include <engine/physics/TransformComponent.h>
#include <SFML/Graphics.hpp>
#include "PaddleController.h"
#include "BallController.h"

int main()
{
    engine::Application app("Pong", 800, 600);
    engine::ecs::World& world = app.getWorld();
    app.setVSync(true);

    auto leftPaddle = world.createEntity();
    leftPaddle.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(30.f, 250.f), 0.f, sf::Vector2f(1.f, 1.f)
    );
    leftPaddle.addComponent<engine::graphics::SpriteComponent>(
        sf::Vector2f(15.f, 100.f), sf::Color::White
    );
    leftPaddle.addComponent<examples::pong::PaddleComponent>(
        sf::Keyboard::W, sf::Keyboard::S, false
    );

    auto rightPaddle = world.createEntity();
    rightPaddle.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(755.f, 250.f), 0.f, sf::Vector2f(1.f, 1.f)
    );
    rightPaddle.addComponent<engine::graphics::SpriteComponent>(
        sf::Vector2f(15.f, 100.f), sf::Color::White
    );
    rightPaddle.addComponent<examples::pong::PaddleComponent>(
        sf::Keyboard::Up, sf::Keyboard::Down, true
    );

    auto ball = world.createEntity();
    ball.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(392.f, 292.f), 0.f, sf::Vector2f(1.f, 1.f)
    );
    ball.addComponent<engine::graphics::SpriteComponent>(
        sf::Vector2f(16.f, 16.f), sf::Color::White
    );
    ball.addComponent<examples::pong::BallComponent>();

    world.registerSystem<examples::pong::PaddleController>();
    world.registerSystem<examples::pong::BallController>();

    app.setFixedUpdate([&](float dt) {
        world.update(dt);
    });

    app.setRender([&](sf::RenderWindow& window) {
        window.clear(sf::Color::Black);
        world.render(window);

        sf::RectangleShape midline(sf::Vector2f(4.f, 600.f));
        midline.setPosition(398.f, 0.f);
        midline.setFillColor(sf::Color(50, 50, 50));
        window.draw(midline);
    });

    app.run();
    return 0;
}
