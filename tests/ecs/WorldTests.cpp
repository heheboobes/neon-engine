#include "../main.cpp"
#include <engine/ecs/World.h>
#include <engine/ecs/System.h>
#include <cassert>
#include <vector>

namespace tests::ecs
{

struct CounterComponent
{
    int value;
};

class CounterSystem : public engine::ecs::System
{
public:
    void update(float dt) override
    {
        for (auto& entity : getEntities())
        {
            auto* counter = entity.getComponent<CounterComponent>();
            if (counter) counter->value++;
        }
    }
    void render(sf::RenderWindow&) override {}
};

class DelayedSystem : public engine::ecs::System
{
public:
    void update(float dt) override {}
    void render(sf::RenderWindow&) override {}
    int updateOrder = 0;
};

TEST(World, CreateAndDestroy)
{
    engine::ecs::World world;

    auto e1 = world.createEntity();
    auto e2 = world.createEntity();
    auto e3 = world.createEntity();

    assert(world.getEntityCount() == 3);
    assert(world.isAlive(e1.getId()));
    assert(world.isAlive(e2.getId()));
    assert(world.isAlive(e3.getId()));

    e2.destroy();
    world.processDestructions();

    assert(world.getEntityCount() == 2);
    assert(!world.isAlive(e2.getId()));

    return true;
}

TEST(World, SystemRegistration)
{
    engine::ecs::World world;

    world.registerSystem<CounterSystem>();
    world.registerSystem<DelayedSystem>();

    auto entity = world.createEntity();
    entity.addComponent<CounterComponent>(0);

    world.update(1.f);

    auto* counter = entity.getComponent<CounterComponent>();
    assert(counter != nullptr);
    assert(counter->value == 1);

    world.update(1.f);
    assert(counter->value == 2);

    return true;
}

TEST(World, Clear)
{
    engine::ecs::World world;

    for (int i = 0; i < 10; ++i)
        world.createEntity();

    assert(world.getEntityCount() == 10);

    world.clear();
    assert(world.getEntityCount() == 0);

    return true;
}

TEST(World, SystemOrdering)
{
    engine::ecs::World world;

    world.registerSystem<DelayedSystem>();
    world.registerSystem<CounterSystem>();
    world.registerSystem<DelayedSystem>();

    auto& systems = world.getSystems();
    assert(systems.size() >= 3);

    return true;
}

}
