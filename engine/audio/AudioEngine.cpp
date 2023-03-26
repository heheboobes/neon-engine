#include "AudioEngine.h"
#include <cmath>

namespace engine::audio {

AudioEngine& AudioEngine::instance() {
    static AudioEngine engine;
    return engine;
}

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::init(const AudioConfig& config) {
    if (m_initialized) {
        return true;
    }
    m_config = config;

    sf::Listener::setGlobalVolume(m_config.masterVolume);
    sf::Listener::setPosition(m_config.listenerPosition);
    sf::Listener::setDirection(m_config.listenerForward);
    sf::Listener::setUpVector(m_config.listenerUp);
    sf::Listener::setDopplerFactor(m_config.dopplerFactor);
    sf::Listener::setSpeedOfSound(m_config.speedOfSound);

    for (int i = 0; i < static_cast<int>(EffectSlot::Count); ++i) {
        m_effectStates[static_cast<EffectSlot>(i)] = false;
    }

    m_initialized = true;
    return true;
}

void AudioEngine::shutdown() {
    if (!m_initialized) return;
    sf::Listener::setGlobalVolume(0.0f);
    m_effectStates.clear();
    m_initialized = false;
}

bool AudioEngine::isInitialized() const {
    return m_initialized;
}

void AudioEngine::setMasterVolume(float volume) {
    volume = std::clamp(volume, 0.0f, 100.0f);
    m_config.masterVolume = volume;
    sf::Listener::setGlobalVolume(volume);
}

float AudioEngine::getMasterVolume() const {
    return m_config.masterVolume;
}

void AudioEngine::setSfxVolume(float volume) {
    m_config.sfxVolume = std::clamp(volume, 0.0f, 100.0f);
}

float AudioEngine::getSfxVolume() const {
    return m_config.sfxVolume;
}

void AudioEngine::setMusicVolume(float volume) {
    m_config.musicVolume = std::clamp(volume, 0.0f, 100.0f);
}

float AudioEngine::getMusicVolume() const {
    return m_config.musicVolume;
}

void AudioEngine::setListenerPosition(const sf::Vector3f& position) {
    m_config.listenerPosition = position;
    sf::Listener::setPosition(position);
}

void AudioEngine::setListenerDirection(const sf::Vector3f& forward, const sf::Vector3f& up) {
    m_config.listenerForward = forward;
    m_config.listenerUp = up;
    sf::Listener::setDirection(forward);
    sf::Listener::setUpVector(up);
}

sf::Vector3f AudioEngine::getListenerPosition() const {
    return m_config.listenerPosition;
}

void AudioEngine::setDopplerFactor(float factor) {
    m_config.dopplerFactor = std::max(0.0f, factor);
    sf::Listener::setDopplerFactor(m_config.dopplerFactor);
}

void AudioEngine::setSpeedOfSound(float speed) {
    m_config.speedOfSound = std::max(0.1f, speed);
    sf::Listener::setSpeedOfSound(m_config.speedOfSound);
}

bool AudioEngine::setEffectSend(EffectSlot slot, bool enabled) {
    if (!m_initialized) return false;
    m_effectStates[slot] = enabled;
    return true;
}

bool AudioEngine::isEffectActive(EffectSlot slot) const {
    auto it = m_effectStates.find(slot);
    return it != m_effectStates.end() && it->second;
}

void AudioEngine::update(float dt) {
    (void)dt;
}

void AudioEngine::setErrorCallback(ErrorCallback callback) {
    m_errorCallback = std::move(callback);
}

} // namespace engine::audio

