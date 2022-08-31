#include "Particle.h"

namespace engine::particles {

Particle::Particle()
    : position(0.f, 0.f)
    , velocity(0.f, 0.f)
    , life(0.f)
    , maxLife(1.f)
    , color(sf::Color::White)
    , startColor(sf::Color::White)
    , endColor(sf::Color(0, 0, 0, 0))
    , size(8.f)
    , startSize(8.f)
    , endSize(2.f)
    , rotation(0.f)
    , rotationSpeed(0.f)
    , uvOffset(0.f, 0.f)
    , uvSize(1.f, 1.f)
    , alive(false)
{
}

void Particle::update(float dt) {
    if (!alive) return;

    life -= dt;
    if (life <= 0.f) {
        alive = false;
        life = 0.f;
        return;
    }

    position += velocity * dt;
    rotation += rotationSpeed * dt;
    color = getInterpolatedColor();
    size = getInterpolatedSize();
}

float Particle::getLifeRatio() const {
    if (maxLife <= 0.f) return 0.f;
    return life / maxLife;
}

sf::Color Particle::getInterpolatedColor() const {
    float t = 1.f - getLifeRatio();
    t = std::clamp(t, 0.f, 1.f);

    sf::Uint8 r = static_cast<sf::Uint8>(
        std::lerp(static_cast<float>(startColor.r),
                  static_cast<float>(endColor.r), t));
    sf::Uint8 g = static_cast<sf::Uint8>(
        std::lerp(static_cast<float>(startColor.g),
                  static_cast<float>(endColor.g), t));
    sf::Uint8 b = static_cast<sf::Uint8>(
        std::lerp(static_cast<float>(startColor.b),
                  static_cast<float>(endColor.b), t));
    sf::Uint8 a = static_cast<sf::Uint8>(
        std::lerp(static_cast<float>(startColor.a),
                  static_cast<float>(endColor.a), t));

    return sf::Color(r, g, b, a);
}

float Particle::getInterpolatedSize() const {
    float t = 1.f - getLifeRatio();
    t = std::clamp(t, 0.f, 1.f);
    return std::lerp(startSize, endSize, t);
}

} // namespace engine::particles
