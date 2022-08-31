#pragma once

#include <SFML/Graphics.hpp>

namespace engine::particles {

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float life;
    float maxLife;
    sf::Color color;
    sf::Color startColor;
    sf::Color endColor;
    float size;
    float startSize;
    float endSize;
    float rotation;
    float rotationSpeed;
    sf::Vector2f uvOffset;
    sf::Vector2f uvSize;
    bool alive;

    Particle();

    void update(float dt);
    float getLifeRatio() const;
    sf::Color getInterpolatedColor() const;
    float getInterpolatedSize() const;
};

} // namespace engine::particles
