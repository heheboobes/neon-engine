#include "../main.cpp"
#include <engine/ecs/Entity.h>
#include <engine/ecs/World.h>
#include <cassert>

namespace tests::ecs
{

struct PositionComponent
{
    float x, y;
};

struct VelocityComponent
{
    float vx, vy;
};

struct TagComponent
{
    std::string tag;
};

TEST(Entity, Create)
{
    engine::ecs::World world;
    engine::ecs::Entity entity = world.createEntity();

    entity.addComponent<PositionComponent>(10.f, 20.f);
    auto* pos = entity.getComponent<PositionComponent>();
    assert(pos != nullptr);
    assert(pos->x == 10.f);
    assert(pos->y == 20.f);

    return true;
}

TEST(Entity, AddAndRemoveComponent)
{
    engine::ecs::World world;
    engine::ecs::Entity entity = world.createEntity();

    entity.addComponent<PositionComponent>(1.f, 2.f);
    entity.addComponent<VelocityComponent>(3.f, 4.f);

    assert(entity.hasComponent<PositionComponent>());
    assert(entity.hasComponent<VelocityComponent>());

    entity.removeComponent<PositionComponent>();
    assert(!entity.hasComponent<PositionComponent>());
    assert(entity.hasComponent<VelocityComponent>());

    auto* vel = entity.getComponent<VelocityComponent>();
    assert(vel != nullptr);
    assert(vel->vx == 3.f);

    return true;
}

TEST(Entity, MultipleComponents)
{
    engine::ecs::World world;
    engine::ecs::Entity entity = world.createEntity();

    entity.addComponent<PositionComponent>(5.f, 10.f);
    entity.addComponent<VelocityComponent>(1.f, 2.f);
    entity.addComponent<TagComponent>("player");

    auto* tag = entity.getComponent<TagComponent>();
    assert(tag != nullptr);
    assert(tag->tag == "player");

    return true;
}

TEST(Entity, Destruction)
{
    engine::ecs::World world;
    engine::ecs::EntityId id;

    {
        engine::ecs::Entity entity = world.createEntity();
        entity.addComponent<PositionComponent>(0.f, 0.f);
        id = entity.getId();
        assert(world.isAlive(id));
    }

    world.processDestructions();
    assert(!world.isAlive(id));

    return true;
}

TEST(Entity, DefaultComponents)
{
    engine::ecs::World world;
    engine::ecs::Entity entity = world.createEntity();

    assert(!entity.hasComponent<PositionComponent>());
    entity.addComponent<PositionComponent>(0.f, 0.f);
    assert(entity.hasComponent<PositionComponent>());

    return true;
}

}
