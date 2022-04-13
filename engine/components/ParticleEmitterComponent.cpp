#include "ParticleEmitterComponent.h"
#include <random>
#include <algorithm>
#include <cmath>

namespace engine {
namespace components {

static std::mt19937 s_rng(std::random_device{}());

ParticleEmitterComponent::ParticleEmitterComponent()
    : maxParticles(100)
    , emissionRate(10.0f)
    , emissionTimer(0.0f)
    , particleLifetimeMin(0.5f)
    , particleLifetimeMax(2.0f)
    , particleSpeedMin(50.0f)
    , particleSpeedMax(200.0f)
    , particleSizeMin(4.0f)
    , particleSizeMax(8.0f)
    , particleRotationMin(0.0f)
    , particleRotationMax(360.0f)
    , colorStart(sf::Color::White)
    , colorEnd(sf::Color(255, 255, 255, 0))
    , emissionOffset(0.f, 0.f)
    , emissionAngleRange(0.f, 360.f)
    , emitting(false)
    , oneShot(false)
    , worldSpace(true)
{
    particles.reserve(maxParticles);
}

void ParticleEmitterComponent::Emit(uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i) {
        EmitParticle();
    }
}

void ParticleEmitterComponent::Burst(uint32_t count)
{
    Emit(count);
}

void ParticleEmitterComponent::Start()
{
    emitting = true;
    emissionTimer = 0.0f;
}

void ParticleEmitterComponent::Stop()
{
    emitting = false;
}

void ParticleEmitterComponent::Update(float deltaTime)
{
    for (auto& p : particles) {
        if (p.alive) {
            UpdateParticle(p, deltaTime);
        }
    }

    if (emitting && !oneShot) {
        emissionTimer += deltaTime;
        float interval = 1.0f / std::max(0.001f, emissionRate);
        while (emissionTimer >= interval) {
            emissionTimer -= interval;
            EmitParticle();
        }
    }
}

void ParticleEmitterComponent::SetRate(float rate)
{
    emissionRate = std::max(0.0f, rate);
}

void ParticleEmitterComponent::SetLifetime(float minLife, float maxLife)
{
    particleLifetimeMin = std::max(0.01f, minLife);
    particleLifetimeMax = std::max(particleLifetimeMin, maxLife);
}

void ParticleEmitterComponent::SetSpeed(float minSpeed, float maxSpeed)
{
    particleSpeedMin = std::max(0.0f, minSpeed);
    particleSpeedMax = std::max(particleSpeedMin, maxSpeed);
}

void ParticleEmitterComponent::SetSize(float minSize, float maxSize)
{
    particleSizeMin = std::max(0.1f, minSize);
    particleSizeMax = std::max(particleSizeMin, maxSize);
}

void ParticleEmitterComponent::SetColors(const sf::Color& start, const sf::Color& end)
{
    colorStart = start;
    colorEnd = end;
}

void ParticleEmitterComponent::SetEmissionAngle(float minDeg, float maxDeg)
{
    emissionAngleRange = sf::Vector2f(minDeg, maxDeg);
}

uint32_t ParticleEmitterComponent::GetAliveCount() const
{
    uint32_t count = 0;
    for (const auto& p : particles) {
        if (p.alive) ++count;
    }
    return count;
}

void ParticleEmitterComponent::Clear()
{
    particles.clear();
}

void ParticleEmitterComponent::EmitParticle()
{
    if (particles.size() >= maxParticles) return;

    Particle p;
    p.position = emissionOffset;
    p.lifetime = RandomRange(particleLifetimeMin, particleLifetimeMax);
    p.maxLifetime = p.lifetime;
    p.age = 0.0f;
    p.alive = true;

    float speed = RandomRange(particleSpeedMin, particleSpeedMax);
    float angleDeg = RandomRange(emissionAngleRange.x, emissionAngleRange.y);
    float angleRad = angleDeg * 3.14159265f / 180.0f;
    p.velocity = sf::Vector2f(std::cos(angleRad) * speed, std::sin(angleRad) * speed);

    p.startColor = colorStart;
    p.endColor = colorEnd;
    p.color = colorStart;

    p.startSize = RandomRange(particleSizeMin, particleSizeMax);
    p.endSize = p.startSize * 0.3f;
    p.size = p.startSize;

    p.rotation = RandomRange(particleRotationMin, particleRotationMax);
    p.rotationSpeed = RandomRange(-180.0f, 180.0f);

    particles.push_back(p);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float deltaTime)
{
    p.age += deltaTime;
    if (p.age >= p.lifetime) {
        p.alive = false;
        return;
    }

    float t = p.age / p.lifetime;
    p.position += p.velocity * deltaTime;
    p.rotation += p.rotationSpeed * deltaTime;

    uint8_t r = static_cast<uint8_t>(std::lerp(p.startColor.r, p.endColor.r, t));
    uint8_t g = static_cast<uint8_t>(std::lerp(p.startColor.g, p.endColor.g, t));
    uint8_t b = static_cast<uint8_t>(std::lerp(p.startColor.b, p.endColor.b, t));
    uint8_t a = static_cast<uint8_t>(std::lerp(p.startColor.a, p.endColor.a, t));
    p.color = sf::Color(r, g, b, a);

    p.size = std::lerp(p.startSize, p.endSize, t);
}

float ParticleEmitterComponent::RandomRange(float min, float max) const
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(s_rng);
}

} // namespace components
} // namespace engine
