#include "../main.cpp"
#include <engine/ecs/World.h>
#include <engine/ecs/System.h>
#include <algorithm>
#include <cassert>

namespace tests::ecs
{

struct HealthComponent
{
    int hp;
    int maxHp;
};

struct ManaComponent
{
    int mana;
    int maxMana;
};

class HealthSystem : public engine::ecs::System
{
public:
    int updateCount = 0;
    int renderCount = 0;

    void update(float dt) override
    {
        updateCount++;
        for (auto& entity : getEntities())
        {
            auto* health = entity.getComponent<HealthComponent>();
            if (health && health->hp > health->maxHp)
                health->hp = health->maxHp;
        }
    }

    void render(sf::RenderWindow& window) override
    {
        renderCount++;
    }
};

class ManaSystem : public engine::ecs::System
{
public:
    float totalDt = 0.f;

    void update(float dt) override
    {
        totalDt += dt;
        for (auto& entity : getEntities())
        {
            auto* mana = entity.getComponent<ManaComponent>();
            if (mana) mana->mana = std::min(mana->mana + 1, mana->maxMana);
        }
    }

    void render(sf::RenderWindow&) override {}
};

TEST(System, UpdateCalled)
{
    engine::ecs::World world;
    auto& sys = world.registerSystem<HealthSystem>();

    auto entity = world.createEntity();
    entity.addComponent<HealthComponent>(100, 100);

    world.update(1.f);
    assert(sys.updateCount == 1);

    world.update(1.f);
    assert(sys.updateCount == 2);

    return true;
}

TEST(System, RenderCalled)
{
    engine::ecs::World world;
    auto& sys = world.registerSystem<HealthSystem>();

    sf::RenderWindow window(sf::VideoMode(100, 100), "Test", sf::Style::None);
    world.render(window);
    assert(sys.renderCount >= 1);

    window.close();
    return true;
}

TEST(System, ComponentModification)
{
    engine::ecs::World world;
    world.registerSystem<ManaSystem>();

    auto entity = world.createEntity();
    entity.addComponent<ManaComponent>(0, 10);

    for (int i = 0; i < 5; ++i)
        world.update(1.f);

    auto* mana = entity.getComponent<ManaComponent>();
    assert(mana != nullptr);
    assert(mana->mana == 5);

    return true;
}

TEST(System, ClampingBehavior)
{
    engine::ecs::World world;
    world.registerSystem<HealthSystem>();

    auto entity = world.createEntity();
    entity.addComponent<HealthComponent>(200, 100);

    world.update(1.f);

    auto* health = entity.getComponent<HealthComponent>();
    assert(health != nullptr);
    assert(health->hp == 100);

    return true;
}

TEST(System, MultipleSystems)
{
    engine::ecs::World world;
    world.registerSystem<HealthSystem>();
    world.registerSystem<ManaSystem>();

    auto entity = world.createEntity();
    entity.addComponent<HealthComponent>(150, 100);
    entity.addComponent<ManaComponent>(5, 20);

    world.update(1.f);

    auto* health = entity.getComponent<HealthComponent>();
    auto* mana = entity.getComponent<ManaComponent>();
    assert(health->hp == 100);
    assert(mana->mana == 6);

    return true;
}

}
