#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <functional>

namespace engine::audio {

enum class EffectSlot {
    Reverb,
    Chorus,
    Flanger,
    Distortion,
    Count
};

struct AudioConfig {
    float masterVolume = 100.0f;
    float sfxVolume = 100.0f;
    float musicVolume = 80.0f;
    sf::Vector3f listenerPosition = {0.0f, 0.0f, 0.0f};
    sf::Vector3f listenerForward = {0.0f, 0.0f, -1.0f};
    sf::Vector3f listenerUp = {0.0f, 1.0f, 0.0f};
    float dopplerFactor = 1.0f;
    float speedOfSound = 343.0f;
};

class AudioEngine {
public:
    static AudioEngine& instance();

    bool init(const AudioConfig& config = AudioConfig{});
    void shutdown();
    bool isInitialized() const;

    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void setSfxVolume(float volume);
    float getSfxVolume() const;
    void setMusicVolume(float volume);
    float getMusicVolume() const;

    void setListenerPosition(const sf::Vector3f& position);
    void setListenerDirection(const sf::Vector3f& forward, const sf::Vector3f& up);
    sf::Vector3f getListenerPosition() const;

    void setDopplerFactor(float factor);
    void setSpeedOfSound(float speed);

    bool setEffectSend(EffectSlot slot, bool enabled);
    bool isEffectActive(EffectSlot slot) const;

    void update(float dt);

    using ErrorCallback = std::function<void(const std::string&)>;
    void setErrorCallback(ErrorCallback callback);

private:
    AudioEngine() = default;
    ~AudioEngine();
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    AudioConfig m_config;
    bool m_initialized = false;
    std::unordered_map<EffectSlot, bool> m_effectStates;
    ErrorCallback m_errorCallback;
};

} // namespace engine::audio
// DOCS: Audio API docs
