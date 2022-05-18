#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/ParticleEmitterComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <memory>

namespace engine {
namespace systems {

class ParticleSystem : public ecs::ISystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnRender(ecs::World* world, float alpha) override;

    void SetMaxParticles(uint32_t max);
    void SetParticleTexture(std::shared_ptr<sf::Texture> texture);
    void SetWorldSpace(bool worldSpace);

private:
    void UpdateEmitter(ecs::World* world, ecs::Entity entity,
                       components::ParticleEmitterComponent& emitter,
                       const components::TransformComponent* transform,
                       float dt);

    void UpdateParticle(components::Particle& p, float dt, bool worldSpace);
    void KillParticle(components::Particle& p);
    void SpawnParticle(components::ParticleEmitterComponent& emitter,
                       const components::TransformComponent* transform);
    void BuildVertexBuffer(const components::ParticleEmitterComponent& emitter,
                           sf::VertexArray& outVertices);

    uint32_t m_maxParticles;
    std::shared_ptr<sf::Texture> m_particleTexture;
    sf::VertexArray m_vertexBuffer;
    bool m_worldSpace;
    float m_particleZ;
};

} // namespace systems
} // namespace engine
