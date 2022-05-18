#include "ParticleSystem.h"
#include "../ecs/World.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <cmath>
#include <cstdlib>

namespace engine {
namespace systems {

ParticleSystem::ParticleSystem()
    : m_maxParticles(1000)
    , m_worldSpace(true)
    , m_particleZ(0.0f)
{
    SetName("ParticleSystem");
    SetPriority(40);
    Require<components::ParticleEmitterComponent>();

    m_vertexBuffer.setPrimitiveType(sf::Quads);
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::SetMaxParticles(uint32_t max)
{
    m_maxParticles = max;
}

void ParticleSystem::SetParticleTexture(std::shared_ptr<sf::Texture> texture)
{
    m_particleTexture = texture;
}

void ParticleSystem::SetWorldSpace(bool worldSpace)
{
    m_worldSpace = worldSpace;
}

void ParticleSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    if (deltaTime <= 0.0f)
        return;

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* emitters = static_cast<components::ParticleEmitterComponent*>(
                chunk.GetComponentData(components::ParticleEmitterComponent::GetStaticTypeID()));
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!emitters)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& emitter = emitters[i];
                const components::TransformComponent* transform =
                    (transforms && i < chunk.GetEntityCount()) ? &transforms[i] : nullptr;

                UpdateEmitter(world, chunk.GetEntity(i), emitter, transform, deltaTime);
            }
        }
    }
}

void ParticleSystem::OnRender(ecs::World* world, float alpha)
{
    (void)world;
    (void)alpha;

    if (m_vertexBuffer.getVertexCount() == 0)
        return;
}

void ParticleSystem::UpdateEmitter(ecs::World* world, ecs::Entity entity,
                                    components::ParticleEmitterComponent& emitter,
                                    const components::TransformComponent* transform,
                                    float dt)
{
    (void)world;
    (void)entity;

    if (!emitter.emitting)
    {
        for (auto& p : emitter.particles)
        {
            if (p.alive)
                UpdateParticle(p, dt, m_worldSpace);
        }
        return;
    }

    emitter.emissionTimer += dt;
    float emitInterval = 1.0f / std::max(emitter.emissionRate, 0.001f);

    while (emitter.emissionTimer >= emitInterval && emitter.particles.size() < m_maxParticles)
    {
        emitter.emissionTimer -= emitInterval;
        SpawnParticle(emitter, transform);
    }

    for (auto it = emitter.particles.begin(); it != emitter.particles.end(); )
    {
        if (it->alive)
        {
            UpdateParticle(*it, dt, m_worldSpace);
            if (it->age >= it->maxLifetime)
            {
                KillParticle(*it);
            }
            ++it;
        }
        else
        {
            it = emitter.particles.erase(it);
        }
    }

    BuildVertexBuffer(emitter, m_vertexBuffer);
}

void ParticleSystem::UpdateParticle(components::Particle& p, float dt, bool worldSpace)
{
    (void)worldSpace;
    p.age += dt;
    float t = std::min(p.age / p.maxLifetime, 1.0f);

    p.velocity.y += 100.0f * dt;
    p.position += p.velocity * dt;
    p.rotation += p.rotationSpeed * dt;

    p.color.r = static_cast<uint8_t>(static_cast<float>(p.startColor.r) * (1.0f - t) +
                                     static_cast<float>(p.endColor.r) * t);
    p.color.g = static_cast<uint8_t>(static_cast<float>(p.startColor.g) * (1.0f - t) +
                                     static_cast<float>(p.endColor.g) * t);
    p.color.b = static_cast<uint8_t>(static_cast<float>(p.startColor.b) * (1.0f - t) +
                                     static_cast<float>(p.endColor.b) * t);
    p.color.a = static_cast<uint8_t>(static_cast<float>(p.startColor.a) * (1.0f - t) +
                                     static_cast<float>(p.endColor.a) * t);

    p.size = p.startSize + (p.endSize - p.startSize) * t;
}

void ParticleSystem::KillParticle(components::Particle& p)
{
    p.alive = false;
}

void ParticleSystem::SpawnParticle(components::ParticleEmitterComponent& emitter,
                                    const components::TransformComponent* transform)
{
    components::Particle p;
    p.alive = true;
    p.age = 0.0f;

    float baseX = 0.0f;
    float baseY = 0.0f;
    if (transform && emitter.worldSpace)
    {
        baseX = transform->position.x;
        baseY = transform->position.y;
    }

    p.position = sf::Vector2f(
        baseX + emitter.emissionOffset.x + emitter.RandomRange(-10.0f, 10.0f),
        baseY + emitter.emissionOffset.y + emitter.RandomRange(-10.0f, 10.0f)
    );

    float angle = emitter.RandomRange(emitter.emissionAngleRange.x, emitter.emissionAngleRange.y);
    float speed = emitter.RandomRange(emitter.particleSpeedMin, emitter.particleSpeedMax);
    float rad = angle * 3.14159f / 180.0f;
    p.velocity = sf::Vector2f(std::cos(rad) * speed, std::sin(rad) * speed);

    p.lifetime = emitter.RandomRange(emitter.particleLifetimeMin, emitter.particleLifetimeMax);
    p.maxLifetime = p.lifetime;
    p.startColor = emitter.colorStart;
    p.endColor = emitter.colorEnd;
    p.color = emitter.colorStart;

    p.startSize = emitter.RandomRange(emitter.particleSizeMin, emitter.particleSizeMax);
    p.endSize = p.startSize * 0.5f;
    p.size = p.startSize;

    p.rotation = emitter.RandomRange(emitter.particleRotationMin, emitter.particleRotationMax);
    p.rotationSpeed = emitter.RandomRange(-180.0f, 180.0f);

    emitter.particles.push_back(p);
}

void ParticleSystem::BuildVertexBuffer(const components::ParticleEmitterComponent& emitter,
                                        sf::VertexArray& outVertices)
{
    uint32_t aliveCount = 0;
    for (const auto& p : emitter.particles)
    {
        if (p.alive) aliveCount++;
    }

    outVertices.resize(aliveCount * 4);
    uint32_t vertexIndex = 0;

    for (const auto& p : emitter.particles)
    {
        if (!p.alive)
            continue;

        float halfSize = p.size * 0.5f;
        float cosR = std::cos(p.rotation * 3.14159f / 180.0f);
        float sinR = std::sin(p.rotation * 3.14159f / 180.0f);

        sf::Vector2f corners[4] = {
            sf::Vector2f(-halfSize, -halfSize),
            sf::Vector2f( halfSize, -halfSize),
            sf::Vector2f( halfSize,  halfSize),
            sf::Vector2f(-halfSize,  halfSize)
        };

        for (int j = 0; j < 4; ++j)
        {
            float rx = corners[j].x * cosR - corners[j].y * sinR;
            float ry = corners[j].x * sinR + corners[j].y * cosR;
            outVertices[vertexIndex + j].position = sf::Vector2f(p.position.x + rx, p.position.y + ry);
            outVertices[vertexIndex + j].color = p.color;
        }

        if (m_particleTexture)
        {
            sf::Vector2f texSize(static_cast<float>(m_particleTexture->getSize().x),
                                 static_cast<float>(m_particleTexture->getSize().y));
            outVertices[vertexIndex + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
            outVertices[vertexIndex + 1].texCoords = sf::Vector2f(texSize.x, 0.0f);
            outVertices[vertexIndex + 2].texCoords = sf::Vector2f(texSize.x, texSize.y);
            outVertices[vertexIndex + 3].texCoords = sf::Vector2f(0.0f, texSize.y);
        }

        vertexIndex += 4;
    }
}

} // namespace systems
} // namespace engine
