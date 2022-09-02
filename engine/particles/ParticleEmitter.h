#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "Particle.h"

namespace engine::particles {

enum class EmitterShape {
    Point,
    Circle,
    Rectangle,
    Cone
};

struct EmissionParams {
    float rate;                
    float rateAccumulator;     
    int burstCount;            
    bool bursting;             
    float burstInterval;       
    float burstTimer;          
    int burstRemaining;        

    EmissionParams();
};

class ParticleEmitter {
public:
    ParticleEmitter();

    void setPosition(const sf::Vector2f& pos);
    void setShape(EmitterShape shape);
    void setEmissionRate(float particlesPerSecond);
    void setBurst(int count, float interval = 0.f);
    void setVelocityRange(float minSpeed, float maxSpeed);
    void setSpread(float angleDegrees);
    void setLifetimeRange(float minLife, float maxLife);
    void setStartSizeRange(float minSize, float maxSize);
    void setEndSizeRange(float minSize, float maxSize);
    void setStartColor(const sf::Color& color);
    void setEndColor(const sf::Color& color);
    void setRotationSpeedRange(float minSpeed, float maxSpeed);
    void setShapeSize(const sf::Vector2f& size);
    void setTextureRect(const sf::Vector2f& uvOffset, const sf::Vector2f& uvSize);

    const sf::Vector2f& getPosition() const;
    EmitterShape getShape() const;

    void emit(std::vector<Particle>& particles, float dt);
    void emitBurst(std::vector<Particle>& particles);
    void reset();

private:
    sf::Vector2f m_position;
    EmitterShape m_shape;
    sf::Vector2f m_shapeSize;

    EmissionParams m_emission;
    float m_minSpeed;
    float m_maxSpeed;
    float m_spread;
    float m_minLife;
    float m_maxLife;
    float m_minStartSize;
    float m_maxStartSize;
    float m_minEndSize;
    float m_maxEndSize;
    float m_minRotSpeed;
    float m_maxRotSpeed;
    sf::Color m_startColor;
    sf::Color m_endColor;
    sf::Vector2f m_uvOffset;
    sf::Vector2f m_uvSize;

    std::mt19937 m_rng;

    sf::Vector2f getRandomPosition();
    sf::Vector2f getRandomVelocity();
    float randomRange(float min, float max);
};

} // namespace engine::particles
