#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include "../../engine/ecs/Component.h"
#include <vector>
#include <functional>
#include <cstdint>

namespace engine {
namespace components {

struct ParticleSpawnParams {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float size;
    float rotation;
};

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    sf::Color startColor;
    sf::Color endColor;
    float lifetime;
    float maxLifetime;
    float size;
    float startSize;
    float endSize;
    float rotation;
    float rotationSpeed;
    float age;
    bool alive;
};

class ParticleEmitterComponent : public ecs::Component<ParticleEmitterComponent> {
public:
    std::vector<Particle> particles;
    uint32_t maxParticles;
    float emissionRate;
    float emissionTimer;

    float particleLifetimeMin;
    float particleLifetimeMax;
    float particleSpeedMin;
    float particleSpeedMax;
    float particleSizeMin;
    float particleSizeMax;
    float particleRotationMin;
    float particleRotationMax;

    sf::Color colorStart;
    sf::Color colorEnd;
    sf::Vector2f emissionOffset;
    sf::Vector2f emissionAngleRange;

    bool emitting;
    bool oneShot;
    bool worldSpace;

    ParticleEmitterComponent();

    void Emit(uint32_t count = 1);
    void Burst(uint32_t count);
    void Start();
    void Stop();
    void Update(float deltaTime);

    void SetRate(float rate);
    void SetLifetime(float minLife, float maxLife);
    void SetSpeed(float minSpeed, float maxSpeed);
    void SetSize(float minSize, float maxSize);
    void SetColors(const sf::Color& start, const sf::Color& end);
    void SetEmissionAngle(float minDeg, float maxDeg);

    uint32_t GetAliveCount() const;
    void Clear();

private:
    void EmitParticle();
    void UpdateParticle(Particle& p, float deltaTime);
    float RandomRange(float min, float max) const;
};

} // namespace components
} // namespace engine
