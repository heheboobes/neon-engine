#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace engine::audio {

struct SoundInstance {
    std::unique_ptr<sf::Sound> sound;
    std::string bufferId;
    bool inUse = false;
};

class SoundManager {
public:
    explicit SoundManager(size_t poolSize = 32);
    ~SoundManager();

    bool loadBuffer(const std::string& id, const std::string& filepath);
    bool loadBufferFromMemory(const std::string& id, const void* data, std::size_t size);
    void unloadBuffer(const std::string& id);
    bool hasBuffer(const std::string& id) const;
    const sf::SoundBuffer* getBuffer(const std::string& id) const;

    sf::Sound* play(const std::string& bufferId, float volume = 100.0f, float pitch = 1.0f);
    sf::Sound* playAtPosition(const std::string& bufferId, const sf::Vector3f& position, float volume = 100.0f, float pitch = 1.0f);
    void stop(const std::string& bufferId);
    void stopAll();
    void pauseAll();
    void resumeAll();

    void setGlobalSfxVolume(float volume);
    float getGlobalSfxVolume() const;

    void update();

    int getActiveSoundCount() const;
    int getAvailableInstanceCount() const;
    void setPoolSize(size_t size);

private:
    SoundInstance* acquireInstance(const std::string& bufferId);
    void releaseInstance(SoundInstance* instance);
    int countActiveForBuffer(const std::string& bufferId) const;

    std::unordered_map<std::string, sf::SoundBuffer> m_buffers;
    std::vector<std::unique_ptr<SoundInstance>> m_pool;
    std::size_t m_poolSize;
    float m_globalVolume = 100.0f;
};

} // namespace engine::audio
