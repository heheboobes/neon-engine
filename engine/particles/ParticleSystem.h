#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Particle.h"
#include "ParticleEmitter.h"

namespace engine::particles {

class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
    ParticleSystem();
    ~ParticleSystem() override;

    void update(float dt);
    void addEmitter(std::unique_ptr<ParticleEmitter> emitter);
    ParticleEmitter* getEmitter(std::size_t index);
    const ParticleEmitter* getEmitter(std::size_t index) const;
    std::size_t getEmitterCount() const;
    void removeEmitter(std::size_t index);
    void clearEmitters();
    void clearParticles();

    void setTexture(const sf::Texture* texture);
    const sf::Texture* getTexture() const;

    void setParticleLimit(std::size_t limit);
    std::size_t getParticleLimit() const;
    std::size_t getParticleCount() const;

    void sortByDepth(bool enabled);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void buildVertexArray();
    void removeDeadParticles();

    std::vector<Particle> m_particles;
    std::vector<std::unique_ptr<ParticleEmitter>> m_emitters;
    sf::VertexArray m_vertices;
    const sf::Texture* m_texture;
    std::size_t m_particleLimit;
    bool m_sortByDepth;
    bool m_verticesDirty;
};

} // namespace engine::particles
