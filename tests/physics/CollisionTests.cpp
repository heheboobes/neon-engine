#include "../main.cpp"
#include <engine/physics/ColliderComponent.h>
#include <engine/physics/PhysicsSystem.h>
#include <engine/math/Vector2.h>
#include <cmath>
#include <cassert>

namespace tests::physics
{

TEST(Collision, AABBOverlap)
{
    engine::physics::ColliderComponent a(sf::FloatRect(0.f, 0.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent b(sf::FloatRect(5.f, 5.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);

    bool overlap = a.bounds.intersects(b.bounds);
    assert(overlap);

    engine::physics::ColliderComponent c(sf::FloatRect(20.f, 20.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);
    overlap = a.bounds.intersects(c.bounds);
    assert(!overlap);

    return true;
}

TEST(Collision, AABBEdgeTouch)
{
    engine::physics::ColliderComponent a(sf::FloatRect(0.f, 0.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent b(sf::FloatRect(10.f, 0.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);

    bool overlap = a.bounds.intersects(b.bounds);
    assert(overlap);

    engine::physics::ColliderComponent c(sf::FloatRect(10.1f, 0.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);
    overlap = a.bounds.intersects(c.bounds);
    assert(!overlap);

    return true;
}

TEST(Collision, AABBContainment)
{
    engine::physics::ColliderComponent outer(sf::FloatRect(0.f, 0.f, 100.f, 100.f),
                                               engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent inner(sf::FloatRect(25.f, 25.f, 50.f, 50.f),
                                               engine::physics::ColliderType::Static);

    bool overlap = outer.bounds.intersects(inner.bounds);
    assert(overlap);

    return true;
}

TEST(Collision, NoOverlap)
{
    engine::physics::ColliderComponent a(sf::FloatRect(0.f, 0.f, 5.f, 5.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent b(sf::FloatRect(10.f, 10.f, 5.f, 5.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent c(sf::FloatRect(0.f, 10.f, 5.f, 5.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent d(sf::FloatRect(10.f, 0.f, 5.f, 5.f),
                                          engine::physics::ColliderType::Static);

    assert(!a.bounds.intersects(b.bounds));
    assert(!a.bounds.intersects(c.bounds));
    assert(!a.bounds.intersects(d.bounds));

    return true;
}

TEST(Collision, ZeroSize)
{
    engine::physics::ColliderComponent a(sf::FloatRect(5.f, 5.f, 0.f, 0.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent b(sf::FloatRect(5.f, 5.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);

    bool overlap = a.bounds.intersects(b.bounds);
    assert(overlap);

    return true;
}

TEST(Collision, NegativeDimensions)
{
    engine::physics::ColliderComponent a(sf::FloatRect(10.f, 10.f, -10.f, -10.f),
                                          engine::physics::ColliderType::Static);
    engine::physics::ColliderComponent b(sf::FloatRect(0.f, 0.f, 10.f, 10.f),
                                          engine::physics::ColliderType::Static);

    bool overlap = a.bounds.intersects(b.bounds);
    assert(overlap);

    return true;
}

TEST(Collision, ColliderType)
{
    engine::physics::ColliderComponent static_collider(
        sf::FloatRect(0.f, 0.f, 10.f, 10.f),
        engine::physics::ColliderType::Static
    );
    assert(static_collider.type == engine::physics::ColliderType::Static);

    engine::physics::ColliderComponent dynamic_collider(
        sf::FloatRect(0.f, 0.f, 10.f, 10.f),
        engine::physics::ColliderType::Dynamic
    );
    assert(dynamic_collider.type == engine::physics::ColliderType::Dynamic);

    return true;
}

}
