#include "ParticleSystem.h"
#include <algorithm>
#include <cmath>

namespace engine::particles {

ParticleSystem::ParticleSystem()
    : m_vertices(sf::Quads)
    , m_texture(nullptr)
    , m_particleLimit(10000)
    , m_sortByDepth(false)
    , m_verticesDirty(false)
{
}

ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::update(float dt) {
    for (auto& emitter : m_emitters) {
        emitter->emit(m_particles, dt);
    }

    for (auto& p : m_particles) {
        p.update(dt);
    }

    removeDeadParticles();

    if (m_particleLimit > 0 && m_particles.size() > m_particleLimit) {
        m_particles.resize(m_particleLimit);
    }

    m_verticesDirty = true;
}

void ParticleSystem::addEmitter(std::unique_ptr<ParticleEmitter> emitter) {
    m_emitters.push_back(std::move(emitter));
}

ParticleEmitter* ParticleSystem::getEmitter(std::size_t index) {
    if (index >= m_emitters.size()) return nullptr;
    return m_emitters[index].get();
}

const ParticleEmitter* ParticleSystem::getEmitter(std::size_t index) const {
    if (index >= m_emitters.size()) return nullptr;
    return m_emitters[index].get();
}

std::size_t ParticleSystem::getEmitterCount() const { return m_emitters.size(); }

void ParticleSystem::removeEmitter(std::size_t index) {
    if (index < m_emitters.size()) {
        m_emitters.erase(m_emitters.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

void ParticleSystem::clearEmitters() { m_emitters.clear(); }
void ParticleSystem::clearParticles() { m_particles.clear(); m_verticesDirty = true; }

void ParticleSystem::setTexture(const sf::Texture* texture) { m_texture = texture; }
const sf::Texture* ParticleSystem::getTexture() const { return m_texture; }

void ParticleSystem::setParticleLimit(std::size_t limit) { m_particleLimit = limit; }
std::size_t ParticleSystem::getParticleLimit() const { return m_particleLimit; }
std::size_t ParticleSystem::getParticleCount() const { return m_particles.size(); }

void ParticleSystem::sortByDepth(bool enabled) { m_sortByDepth = enabled; }

void ParticleSystem::removeDeadParticles() {
    auto it = std::remove_if(m_particles.begin(), m_particles.end(),
        [](const Particle& p) { return !p.alive; });
    m_particles.erase(it, m_particles.end());
}

void ParticleSystem::buildVertexArray() {
    if (!m_verticesDirty) return;
    m_verticesDirty = false;

    std::size_t count = m_particles.size();
    m_vertices.resize(count * 4);

    for (std::size_t i = 0; i < count; ++i) {
        const auto& p = m_particles[i];
        float half = p.size * 0.5f;

        sf::Vertex* quad = &m_vertices[i * 4];

        quad[0].position = sf::Vector2f(p.position.x - half, p.position.y - half);
        quad[1].position = sf::Vector2f(p.position.x + half, p.position.y - half);
        quad[2].position = sf::Vector2f(p.position.x + half, p.position.y + half);
        quad[3].position = sf::Vector2f(p.position.x - half, p.position.y + half);

        if (p.rotation != 0.f) {
            float rad = p.rotation * 3.14159265f / 180.f;
            float c = std::cos(rad);
            float s = std::sin(rad);
            for (int j = 0; j < 4; ++j) {
                sf::Vector2f local = quad[j].position - p.position;
                quad[j].position = p.position + sf::Vector2f(
                    local.x * c - local.y * s,
                    local.x * s + local.y * c);
            }
        }

        quad[0].texCoords = sf::Vector2f(p.uvOffset.x, p.uvOffset.y);
        quad[1].texCoords = sf::Vector2f(p.uvOffset.x + p.uvSize.x, p.uvOffset.y);
        quad[2].texCoords = sf::Vector2f(p.uvOffset.x + p.uvSize.x, p.uvOffset.y + p.uvSize.y);
        quad[3].texCoords = sf::Vector2f(p.uvOffset.x, p.uvOffset.y + p.uvSize.y);

        quad[0].color = p.color;
        quad[1].color = p.color;
        quad[2].color = p.color;
        quad[3].color = p.color;
    }
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto& self = const_cast<ParticleSystem&>(*this);

    if (m_sortByDepth) {
        std::stable_sort(self.m_particles.begin(), self.m_particles.end(),
            [](const Particle& a, const Particle& b) {
                return a.position.y < b.position.y;
            });
        self.m_verticesDirty = true;
    }

    self.buildVertexArray();

    states.transform *= getTransform();
    if (m_texture) {
        states.texture = m_texture;
    }

    target.draw(m_vertices, states);
}

} // namespace engine::particles
// v2: SIMD optimization hints
// v3: Memory leak fix
