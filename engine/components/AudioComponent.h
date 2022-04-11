#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Vector3.hpp>
#include "../../engine/ecs/Component.h"
#include <memory>
#include <string>

namespace engine {
namespace components {

enum class AudioChannel {
    Master,
    Music,
    SFX,
    Ambient,
    Voice
};

class AudioComponent : public ecs::Component<AudioComponent> {
public:
    std::shared_ptr<sf::SoundBuffer> soundBuffer;
    sf::Sound sound;

    std::string soundId;
    AudioChannel channel;

    float volume;
    float pitch;
    float minDistance;
    float attenuation;

    bool spatialAudio;
    bool looping;
    bool autoPlay;

    AudioComponent();

    void SetSound(const std::string& filepath);
    void SetSound(const std::string& id, std::shared_ptr<sf::SoundBuffer> buffer);
    void SetVolume(float vol);
    void SetPitch(float p);
    void SetLooping(bool loop);

    void Play();
    void Stop();
    void Pause();
    void Resume();

    void SetSpatial(bool enabled);
    void SetMinDistance(float dist);
    void SetAttenuation(float atten);
    void SetPosition(const sf::Vector3f& pos);

    bool IsPlaying() const;
    float GetDuration() const;
    float GetPlaybackOffset() const;
    void SetPlaybackOffset(float time);

    AudioChannel GetChannel() const;
    void SetChannel(AudioChannel ch);

    void Release();
};

} // namespace components
} // namespace engine
