#include "../main.cpp"
#include <engine/physics/PhysicsSystem.h>
#include <engine/physics/TransformComponent.h>
#include <engine/physics/ColliderComponent.h>
#include <engine/ecs/World.h>
#include <cmath>
#include <cassert>

namespace tests::physics
{

struct PhysicsTestSystem : public engine::ecs::System
{
    void update(float dt) override
    {
        for (auto& entity : getEntities())
        {
            auto* transform = entity.getComponent<engine::physics::TransformComponent>();
            auto* collider = entity.getComponent<engine::physics::ColliderComponent>();
            if (transform && collider)
            {
                collider->bounds.left = transform->position.x;
                collider->bounds.top = transform->position.y;
            }
        }
    }
    void render(sf::RenderWindow&) override {}
};

TEST(Physics, TransformMove)
{
    engine::physics::TransformComponent transform(
        sf::Vector2f(100.f, 200.f), 45.f, sf::Vector2f(2.f, 2.f)
    );

    assert(std::abs(transform.position.x - 100.f) < 0.0001f);
    assert(std::abs(transform.position.y - 200.f) < 0.0001f);
    assert(std::abs(transform.rotation - 45.f) < 0.0001f);
    assert(std::abs(transform.scale.x - 2.f) < 0.0001f);
    assert(std::abs(transform.scale.y - 2.f) < 0.0001f);

    transform.position += sf::Vector2f(10.f, 20.f);
    assert(std::abs(transform.position.x - 110.f) < 0.0001f);
    assert(std::abs(transform.position.y - 220.f) < 0.0001f);

    return true;
}

TEST(Physics, Gravity)
{
    engine::ecs::World world;
    world.registerSystem<PhysicsTestSystem>();

    auto entity = world.createEntity();
    entity.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)
    );

    auto* transform = entity.getComponent<engine::physics::TransformComponent>();

    for (int i = 0; i < 60; ++i)
        world.update(1.f / 60.f);

    assert(transform->position.y > 0.f);

    return true;
}

TEST(Physics, ColliderBoundsSync)
{
    engine::ecs::World world;
    world.registerSystem<PhysicsTestSystem>();

    auto entity = world.createEntity();
    entity.addComponent<engine::physics::TransformComponent>(
        sf::Vector2f(50.f, 50.f), 0.f, sf::Vector2f(1.f, 1.f)
    );
    entity.addComponent<engine::physics::ColliderComponent>(
        sf::FloatRect(50.f, 50.f, 32.f, 32.f),
        engine::physics::ColliderType::Dynamic
    );

    auto* transform = entity.getComponent<engine::physics::TransformComponent>();
    auto* collider = entity.getComponent<engine::physics::ColliderComponent>();

    transform->position.x = 100.f;
    world.update(1.f);

    assert(std::abs(collider->bounds.left - 100.f) < 0.0001f);

    return true;
}

TEST(Physics, MultipleBodies)
{
    engine::ecs::World world;
    world.registerSystem<PhysicsTestSystem>();

    for (int i = 0; i < 10; ++i)
    {
        auto entity = world.createEntity();
        entity.addComponent<engine::physics::TransformComponent>(
            sf::Vector2f(static_cast<float>(i * 20), 0.f), 0.f, sf::Vector2f(1.f, 1.f)
        );
        entity.addComponent<engine::physics::ColliderComponent>(
            sf::FloatRect(0.f, 0.f, 16.f, 16.f),
            engine::physics::ColliderType::Dynamic
        );
    }

    world.update(1.f);
    assert(world.getEntityCount() == 10);

    return true;
}

TEST(Physics, StaticVsDynamic)
{
    engine::physics::ColliderComponent staticCol(
        sf::FloatRect(0.f, 0.f, 100.f, 20.f),
        engine::physics::ColliderType::Static
    );
    engine::physics::ColliderComponent dynamicCol(
        sf::FloatRect(40.f, 10.f, 20.f, 20.f),
        engine::physics::ColliderType::Dynamic
    );

    assert(staticCol.type != dynamicCol.type);
    assert(staticCol.bounds.intersects(dynamicCol.bounds));

    dynamicCol.bounds.top = 30.f;
    assert(!staticCol.bounds.intersects(dynamicCol.bounds));

    return true;
}

}
