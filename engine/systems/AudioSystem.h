#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/AudioComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Listener.hpp>
#include <unordered_map>
#include <memory>
#include <queue>
#include <string>

namespace engine {
namespace systems {

struct AudioHandle {
    uint32_t id;
    ecs::Entity entity;
    std::string soundId;
    bool valid;

    AudioHandle() : id(0), valid(false) {}
};

class AudioSystem : public ecs::ISystem {
public:
    AudioSystem();
    ~AudioSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;

    AudioHandle PlaySound(const std::string& soundId, float volume = 1.0f, float pitch = 1.0f);
    void StopAll();
    void PauseAll();
    void ResumeAll();
    void SetMasterVolume(float volume);
    float GetMasterVolume() const;
    void SetChannelVolume(int channel, float volume);
    void SetListenerPosition(const sf::Vector3f& position);
    void SetListenerDirection(const sf::Vector3f& direction);

    void SetMaxSources(int32_t maxSources);

private:
    struct ActiveSource {
        sf::Sound sound;
        ecs::Entity entity;
        std::string soundId;
        AudioChannel channel;
        bool spatial;
        bool valid;
    };

    void UpdateSpatialAudio(ActiveSource& source, const components::TransformComponent* transform);
    void ReclaimStoppedSources();
    ActiveSource* AcquireSource();
    void StopSource(ActiveSource& source);

    std::vector<std::shared_ptr<sf::SoundBuffer>> m_bufferCache;
    std::vector<ActiveSource> m_sources;
    std::queue<uint32_t> m_freeSlots;
    float m_masterVolume;
    float m_channelVolumes[5];
    sf::Vector3f m_listenerPosition;
    int32_t m_maxSources;
    uint32_t m_nextHandleId;
};

} // namespace systems
} // namespace engine
// v2: Object pooling
