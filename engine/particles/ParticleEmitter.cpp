#include "ParticleEmitter.h"
#include <cmath>

namespace engine::particles {

EmissionParams::EmissionParams()
    : rate(100.f)
    , rateAccumulator(0.f)
    , burstCount(0)
    , bursting(false)
    , burstInterval(0.f)
    , burstTimer(0.f)
    , burstRemaining(0)
{
}

ParticleEmitter::ParticleEmitter()
    : m_position(0.f, 0.f)
    , m_shape(EmitterShape::Point)
    , m_shapeSize(32.f, 32.f)
    , m_minSpeed(50.f)
    , m_maxSpeed(150.f)
    , m_spread(360.f)
    , m_minLife(0.5f)
    , m_maxLife(2.f)
    , m_minStartSize(4.f)
    , m_maxStartSize(8.f)
    , m_minEndSize(1.f)
    , m_maxEndSize(3.f)
    , m_minRotSpeed(-180.f)
    , m_maxRotSpeed(180.f)
    , m_startColor(sf::Color::White)
    , m_endColor(sf::Color(255, 255, 255, 0))
    , m_uvOffset(0.f, 0.f)
    , m_uvSize(1.f, 1.f)
    , m_rng(std::random_device{}())
{
}

void ParticleEmitter::setPosition(const sf::Vector2f& pos) { m_position = pos; }
void ParticleEmitter::setShape(EmitterShape shape) { m_shape = shape; }
void ParticleEmitter::setEmissionRate(float rate) { m_emission.rate = rate; }
void ParticleEmitter::setVelocityRange(float min, float max) { m_minSpeed = min; m_maxSpeed = max; }
void ParticleEmitter::setSpread(float angle) { m_spread = angle; }
void ParticleEmitter::setLifetimeRange(float min, float max) { m_minLife = min; m_maxLife = max; }
void ParticleEmitter::setStartSizeRange(float min, float max) { m_minStartSize = min; m_maxStartSize = max; }
void ParticleEmitter::setEndSizeRange(float min, float max) { m_minEndSize = min; m_maxEndSize = max; }
void ParticleEmitter::setStartColor(const sf::Color& c) { m_startColor = c; }
void ParticleEmitter::setEndColor(const sf::Color& c) { m_endColor = c; }
void ParticleEmitter::setRotationSpeedRange(float min, float max) { m_minRotSpeed = min; m_maxRotSpeed = max; }
void ParticleEmitter::setShapeSize(const sf::Vector2f& size) { m_shapeSize = size; }
void ParticleEmitter::setTextureRect(const sf::Vector2f& uv, const sf::Vector2f& uvSz) { m_uvOffset = uv; m_uvSize = uvSz; }

const sf::Vector2f& ParticleEmitter::getPosition() const { return m_position; }
EmitterShape ParticleEmitter::getShape() const { return m_shape; }

float ParticleEmitter::randomRange(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_rng);
}

sf::Vector2f ParticleEmitter::getRandomPosition() {
    switch (m_shape) {
    case EmitterShape::Point:
        return m_position;
    case EmitterShape::Circle: {
        float radius = m_shapeSize.x * 0.5f;
        float angle = randomRange(0.f, 6.2831853f);
        float r = radius * std::sqrt(randomRange(0.f, 1.f));
        return m_position + sf::Vector2f(r * std::cos(angle), r * std::sin(angle));
    }
    case EmitterShape::Rectangle:
        return m_position + sf::Vector2f(
            randomRange(-m_shapeSize.x * 0.5f, m_shapeSize.x * 0.5f),
            randomRange(-m_shapeSize.y * 0.5f, m_shapeSize.y * 0.5f));
    case EmitterShape::Cone: {
        float angle = randomRange(-m_spread * 0.5f, m_spread * 0.5f);
        float dist = randomRange(0.f, m_shapeSize.x);
        float rad = angle * 3.14159265f / 180.f;
        return m_position + sf::Vector2f(dist * std::cos(rad), dist * std::sin(rad));
    }
    default:
        return m_position;
    }
}

sf::Vector2f ParticleEmitter::getRandomVelocity() {
    float angle = randomRange(-m_spread * 0.5f, m_spread * 0.5f);
    float speed = randomRange(m_minSpeed, m_maxSpeed);
    float rad = angle * 3.14159265f / 180.f;
    return sf::Vector2f(speed * std::cos(rad), speed * std::sin(rad));
}

void ParticleEmitter::emit(std::vector<Particle>& particles, float dt) {
    if (m_emission.rate <= 0.f) return;

    m_emission.rateAccumulator += m_emission.rate * dt;
    int count = static_cast<int>(m_emission.rateAccumulator);
    m_emission.rateAccumulator -= static_cast<float>(count);

    m_emission.burstTimer -= dt;
    if (m_emission.bursting && m_emission.burstTimer <= 0.f) {
        if (m_emission.burstRemaining > 0) {
            int toEmit = std::min(m_emission.burstCount, m_emission.burstRemaining);
            for (int i = 0; i < toEmit; ++i) {
                Particle p;
                p.position = getRandomPosition();
                p.velocity = getRandomVelocity();
                p.life = randomRange(m_minLife, m_maxLife);
                p.maxLife = p.life;
                p.startColor = m_startColor;
                p.endColor = m_endColor;
                p.color = m_startColor;
                p.startSize = randomRange(m_minStartSize, m_maxStartSize);
                p.endSize = randomRange(m_minEndSize, m_maxEndSize);
                p.size = p.startSize;
                p.rotation = randomRange(0.f, 360.f);
                p.rotationSpeed = randomRange(m_minRotSpeed, m_maxRotSpeed);
                p.uvOffset = m_uvOffset;
                p.uvSize = m_uvSize;
                p.alive = true;
                particles.push_back(p);
            }
            m_emission.burstRemaining -= toEmit;
            m_emission.burstTimer = m_emission.burstInterval;
            m_emission.bursting = m_emission.burstRemaining > 0;
        }
    }

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = getRandomPosition();
        p.velocity = getRandomVelocity();
        p.life = randomRange(m_minLife, m_maxLife);
        p.maxLife = p.life;
        p.startColor = m_startColor;
        p.endColor = m_endColor;
        p.color = m_startColor;
        p.startSize = randomRange(m_minStartSize, m_maxStartSize);
        p.endSize = randomRange(m_minEndSize, m_maxEndSize);
        p.size = p.startSize;
        p.rotation = randomRange(0.f, 360.f);
        p.rotationSpeed = randomRange(m_minRotSpeed, m_maxRotSpeed);
        p.uvOffset = m_uvOffset;
        p.uvSize = m_uvSize;
        p.alive = true;
        particles.push_back(p);
    }
}

void ParticleEmitter::setBurst(int count, float interval) {
    m_emission.burstCount = count;
    m_emission.burstInterval = interval;
    m_emission.burstTimer = 0.f;
    m_emission.burstRemaining = count;
    m_emission.bursting = count > 0;
}

void ParticleEmitter::emitBurst(std::vector<Particle>& particles) {
    for (int i = 0; i < m_emission.burstCount; ++i) {
        Particle p;
        p.position = getRandomPosition();
        p.velocity = getRandomVelocity();
        p.life = randomRange(m_minLife, m_maxLife);
        p.maxLife = p.life;
        p.startColor = m_startColor;
        p.endColor = m_endColor;
        p.color = m_startColor;
        p.startSize = randomRange(m_minStartSize, m_maxStartSize);
        p.endSize = randomRange(m_minEndSize, m_maxEndSize);
        p.size = p.startSize;
        p.rotation = randomRange(0.f, 360.f);
        p.rotationSpeed = randomRange(m_minRotSpeed, m_maxRotSpeed);
        p.uvOffset = m_uvOffset;
        p.uvSize = m_uvSize;
        p.alive = true;
        particles.push_back(p);
    }
}

void ParticleEmitter::reset() {
    m_emission.rateAccumulator = 0.f;
    m_emission.burstTimer = 0.f;
    m_emission.burstRemaining = 0;
    m_emission.bursting = false;
}

} // namespace engine::particles

