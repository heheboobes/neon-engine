#include "SoundManager.h"
#include <algorithm>
#include <stdexcept>

namespace engine::audio {

SoundManager::SoundManager(size_t poolSize)
    : m_poolSize(poolSize) {
    m_pool.reserve(poolSize);
    for (size_t i = 0; i < poolSize; ++i) {
        auto instance = std::make_unique<SoundInstance>();
        instance->sound = std::make_unique<sf::Sound>();
        instance->inUse = false;
        m_pool.push_back(std::move(instance));
    }
}

SoundManager::~SoundManager() {
    stopAll();
    m_buffers.clear();
}

bool SoundManager::loadBuffer(const std::string& id, const std::string& filepath) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        return false;
    }
    m_buffers[id] = std::move(buffer);
    return true;
}

bool SoundManager::loadBufferFromMemory(const std::string& id, const void* data, std::size_t size) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromMemory(data, size)) {
        return false;
    }
    m_buffers[id] = std::move(buffer);
    return true;
}

void SoundManager::unloadBuffer(const std::string& id) {
    stop(id);
    m_buffers.erase(id);
}

bool SoundManager::hasBuffer(const std::string& id) const {
    return m_buffers.find(id) != m_buffers.end();
}

const sf::SoundBuffer* SoundManager::getBuffer(const std::string& id) const {
    auto it = m_buffers.find(id);
    return it != m_buffers.end() ? &it->second : nullptr;
}

sf::Sound* SoundManager::play(const std::string& bufferId, float volume, float pitch) {
    auto* instance = acquireInstance(bufferId);
    if (!instance) return nullptr;

    instance->sound->setBuffer(*m_buffers[bufferId]);
    instance->sound->setVolume(volume * (m_globalVolume / 100.0f));
    instance->sound->setPitch(pitch);
    instance->sound->setRelativeToListener(true);
    instance->sound->setMinDistance(1.0f);
    instance->sound->setAttenuation(1.0f);
    instance->sound->play();
    return instance->sound.get();
}

sf::Sound* SoundManager::playAtPosition(const std::string& bufferId, const sf::Vector3f& position, float volume, float pitch) {
    auto* instance = acquireInstance(bufferId);
    if (!instance) return nullptr;

    instance->sound->setBuffer(*m_buffers[bufferId]);
    instance->sound->setVolume(volume * (m_globalVolume / 100.0f));
    instance->sound->setPitch(pitch);
    instance->sound->setPosition(position);
    instance->sound->setRelativeToListener(false);
    instance->sound->setMinDistance(1.0f);
    instance->sound->setAttenuation(1.0f);
    instance->sound->play();
    return instance->sound.get();
}

void SoundManager::stop(const std::string& bufferId) {
    for (auto& instance : m_pool) {
        if (instance->inUse && instance->bufferId == bufferId) {
            instance->sound->stop();
            releaseInstance(instance.get());
        }
    }
}

void SoundManager::stopAll() {
    for (auto& instance : m_pool) {
        if (instance->inUse) {
            instance->sound->stop();
            releaseInstance(instance.get());
        }
    }
}

void SoundManager::pauseAll() {
    for (auto& instance : m_pool) {
        if (instance->inUse) {
            instance->sound->pause();
        }
    }
}

void SoundManager::resumeAll() {
    for (auto& instance : m_pool) {
        if (instance->inUse) {
            instance->sound->play();
        }
    }
}

void SoundManager::setGlobalSfxVolume(float volume) {
    m_globalVolume = std::clamp(volume, 0.0f, 100.0f);
}

float SoundManager::getGlobalSfxVolume() const {
    return m_globalVolume;
}

void SoundManager::update() {
    for (auto& instance : m_pool) {
        if (instance->inUse && instance->sound->getStatus() == sf::Sound::Stopped) {
            releaseInstance(instance.get());
        }
    }
}

int SoundManager::getActiveSoundCount() const {
    int count = 0;
    for (const auto& instance : m_pool) {
        if (instance->inUse) ++count;
    }
    return count;
}

int SoundManager::getAvailableInstanceCount() const {
    return static_cast<int>(m_poolSize) - getActiveSoundCount();
}

void SoundManager::setPoolSize(size_t size) {
    if (size < m_pool.size()) return;
    m_poolSize = size;
    for (size_t i = m_pool.size(); i < size; ++i) {
        auto instance = std::make_unique<SoundInstance>();
        instance->sound = std::make_unique<sf::Sound>();
        instance->inUse = false;
        m_pool.push_back(std::move(instance));
    }
}

SoundInstance* SoundManager::acquireInstance(const std::string& bufferId) {
    auto bufIt = m_buffers.find(bufferId);
    if (bufIt == m_buffers.end()) return nullptr;

    for (auto& instance : m_pool) {
        if (!instance->inUse) {
            instance->inUse = true;
            instance->bufferId = bufferId;
            return instance.get();
        }
    }
    return nullptr;
}

void SoundManager::releaseInstance(SoundInstance* instance) {
    instance->inUse = false;
    instance->bufferId.clear();
    instance->sound->stop();
    instance->sound->resetBuffer();
}

int SoundManager::countActiveForBuffer(const std::string& bufferId) const {
    int count = 0;
    for (const auto& instance : m_pool) {
        if (instance->inUse && instance->bufferId == bufferId) ++count;
    }
    return count;
}

} // namespace engine::audio


