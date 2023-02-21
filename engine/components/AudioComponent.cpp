#include "AudioComponent.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace components {

AudioComponent::AudioComponent()
    : soundBuffer(nullptr)
    , channel(AudioChannel::SFX)
    , volume(100.0f)
    , pitch(1.0f)
    , minDistance(10.0f)
    , attenuation(1.0f)
    , spatialAudio(false)
    , looping(false)
    , autoPlay(false)
{
    sound.setVolume(volume);
    sound.setPitch(pitch);
    sound.setMinDistance(minDistance);
    sound.setAttenuation(attenuation);
    sound.setLoop(looping);
}

void AudioComponent::SetSound(const std::string& filepath)
{
    auto buffer = std::make_shared<sf::SoundBuffer>();
    if (buffer->loadFromFile(filepath)) {
        soundBuffer = std::move(buffer);
        sound.setBuffer(*soundBuffer);
        soundId = filepath;
        if (autoPlay) Play();
    }
}

void AudioComponent::SetSound(const std::string& id, std::shared_ptr<sf::SoundBuffer> buffer)
{
    if (buffer) {
        soundId = id;
        soundBuffer = std::move(buffer);
        sound.setBuffer(*soundBuffer);
        if (autoPlay) Play();
    }
}

void AudioComponent::SetVolume(float vol)
{
    volume = std::clamp(vol, 0.0f, 100.0f);
    sound.setVolume(volume);
}

void AudioComponent::SetPitch(float p)
{
    pitch = std::max(0.01f, p);
    sound.setPitch(pitch);
}

void AudioComponent::SetLooping(bool loop)
{
    looping = loop;
    sound.setLoop(looping);
}

void AudioComponent::Play()
{
    if (soundBuffer) {
        sound.play();
    }
}

void AudioComponent::Stop()
{
    sound.stop();
}

void AudioComponent::Pause()
{
    sound.pause();
}

void AudioComponent::Resume()
{
    if (sound.getStatus() == sf::Sound::Paused) {
        sound.play();
    }
}

void AudioComponent::SetSpatial(bool enabled)
{
    spatialAudio = enabled;
    sound.setRelativeToListener(!enabled);
    if (!enabled) {
        sound.setPosition(0.f, 0.f, 0.f);
    }
}

void AudioComponent::SetMinDistance(float dist)
{
    minDistance = std::max(0.0f, dist);
    sound.setMinDistance(minDistance);
}

void AudioComponent::SetAttenuation(float atten)
{
    attenuation = std::max(0.0f, atten);
    sound.setAttenuation(attenuation);
}

void AudioComponent::SetPosition(const sf::Vector3f& pos)
{
    if (spatialAudio) {
        sound.setPosition(pos.x, pos.y, pos.z);
    }
}

bool AudioComponent::IsPlaying() const
{
    return sound.getStatus() == sf::Sound::Playing;
}

float AudioComponent::GetDuration() const
{
    if (soundBuffer) {
        return soundBuffer->getDuration().asSeconds();
    }
    return 0.0f;
}

float AudioComponent::GetPlaybackOffset() const
{
    return sound.getPlayingOffset().asSeconds();
}

void AudioComponent::SetPlaybackOffset(float time)
{
    sound.setPlayingOffset(sf::seconds(std::max(0.0f, time)));
}

AudioChannel AudioComponent::GetChannel() const
{
    return channel;
}

void AudioComponent::SetChannel(AudioChannel ch)
{
    channel = ch;
}

void AudioComponent::Release()
{
    sound.stop();
    sound.resetBuffer();
    soundBuffer.reset();
    soundId.clear();
}

} // namespace components
} // namespace engine


