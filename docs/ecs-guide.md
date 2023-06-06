# ECS Guide

## Core Concepts

The engine uses an **archetype-based ECS**. Entities with identical component sets are stored together in dense, contiguous chunks. This design maximizes cache utilization during system iteration.

## Creating Components

Components are plain structs. No base class, no virtual methods, no reflection macros.

```cpp
struct Health {
    float current;
    float max;
};

struct Damage {
    float amount;
    Entity source;
};
```

## Creating a System

A system is any callable that accepts a `Query` and a `float dt`:

```cpp
// Free function system
void damage_system(ecs::Query<Health, Damage> query, float dt) {
    query.each([](ecs::Entity e, Health& hp, Damage& dmg) {
        hp.current -= dmg.amount * dt;
        if (hp.current <= 0.0f) {
            fmt::print("Entity {} destroyed\n", e.id());
        }
    });
}

world.register_system(damage_system);
```

## Archetypes

Archetypes are created implicitly. The first time you create an entity with `{Transform, Velocity}`, an archetype `{Transform, Velocity}` is allocated. The engine never merges or splits archetypes at runtime.

```cpp
// These two entities share archetype {Transform, Velocity}
auto a = world.create(Transform{}, Velocity{});
auto b = world.create(Transform{}, Velocity{});

// This entity gets its own archetype {Transform, Velocity, Health}
auto c = world.create(Transform{}, Velocity{}, Health{});
```

## Queries

Queries match archetypes that contain **at least** the requested components. Additional components on the archetype are ignored.

```cpp
// Matches all entities with Transform AND Velocity (regardless of other components)
auto q = world.query<Transform, Velocity>();
q.par_each([](Transform& t, Velocity& v) {
    t.position += v.linear * dt;
}, scheduler);
```

Use `ecs::Without<T>` to exclude archetypes that contain a component:

```cpp
// All entities with Transform but WITHOUT StaticTag
auto q = world.query<Transform>(ecs::Without<StaticTag>{});
```

## Component Lifecycle Hooks

Components can define optional hooks:

```cpp
struct Inventory {
    std::vector<Item> items;

    void on_add(ecs::Entity) { /* called when added to entity */ }
    void on_remove(ecs::Entity) { /* called when removed from entity */ }
};
```

## Performance Tips

- Keep components small (< 64 bytes). Split large data into multiple components.
- Prefer SoA-layout by using `ecs::SparseSet<T>` for rarely-accessed components.
- Use `par_each` for read-only queries. Write queries should use `each` unless you manually partition.
