#include "AudioSystem.h"
#include "../ecs/World.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace systems {

AudioSystem::AudioSystem()
    : m_masterVolume(1.0f)
    , m_maxSources(32)
    , m_nextHandleId(1)
{
    SetName("AudioSystem");
    SetPriority(20);

    for (int i = 0; i < 5; ++i)
        m_channelVolumes[i] = 1.0f;

    m_listenerPosition = sf::Vector3f(0.0f, 0.0f, 0.0f);
    m_sources.resize(m_maxSources);
    for (uint32_t i = 0; i < m_maxSources; ++i)
    {
        m_sources[i].valid = false;
        m_freeSlots.push(i);
    }
}

AudioSystem::~AudioSystem()
{
    StopAll();
}

void AudioSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    (void)deltaTime;
    ReclaimStoppedSources();

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* audios = static_cast<components::AudioComponent*>(
                chunk.GetComponentData(components::AudioComponent::GetStaticTypeID()));
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!audios)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& audio = audios[i];

                if (audio.autoPlay && !audio.IsPlaying() && audio.soundBuffer)
                {
                    audio.Play();
                }

                if (audio.IsPlaying())
                {
                    float effectiveVolume = audio.volume * m_masterVolume;
                    int channelIdx = static_cast<int>(audio.channel);
                    if (channelIdx >= 0 && channelIdx < 5)
                        effectiveVolume *= m_channelVolumes[channelIdx];

                    audio.sound.setVolume(effectiveVolume * 100.0f);

                    if (audio.spatialAudio && transforms)
                    {
                        audio.sound.setPosition(
                            transforms->position.x,
                            transforms->position.y,
                            transforms->position.z
                        );
                    }
                }
            }
        }
    }
}

AudioHandle AudioSystem::PlaySound(const std::string& soundId, float volume, float pitch)
{
    AudioHandle handle;
    handle.id = m_nextHandleId++;
    handle.soundId = soundId;
    handle.valid = true;

    ActiveSource* source = AcquireSource();
    if (!source)
    {
        handle.valid = false;
        return handle;
    }

    source->soundId = soundId;
    source->entity = ecs::Entity::Null();
    source->spatial = false;
    source->channel = AudioChannel::SFX;
    source->sound.setVolume(volume * m_masterVolume * 100.0f);
    source->sound.setPitch(pitch);
    source->sound.play();
    source->valid = true;

    return handle;
}

void AudioSystem::StopAll()
{
    for (auto& source : m_sources)
    {
        if (source.valid)
        {
            source.sound.stop();
            source.valid = false;
        }
    }
}

void AudioSystem::PauseAll()
{
    for (auto& source : m_sources)
    {
        if (source.valid)
            source.sound.pause();
    }
}

void AudioSystem::ResumeAll()
{
    for (auto& source : m_sources)
    {
        if (source.valid)
            source.sound.play();
    }
}

void AudioSystem::SetMasterVolume(float volume)
{
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

float AudioSystem::GetMasterVolume() const
{
    return m_masterVolume;
}

void AudioSystem::SetChannelVolume(int channel, float volume)
{
    if (channel >= 0 && channel < 5)
        m_channelVolumes[channel] = std::clamp(volume, 0.0f, 1.0f);
}

void AudioSystem::SetListenerPosition(const sf::Vector3f& position)
{
    m_listenerPosition = position;
    sf::Listener::setPosition(position);
}

void AudioSystem::SetListenerDirection(const sf::Vector3f& direction)
{
    sf::Listener::setDirection(direction);
}

void AudioSystem::SetMaxSources(int32_t maxSources)
{
    m_maxSources = std::max(4, maxSources);
    m_sources.resize(m_maxSources);
}

void AudioSystem::UpdateSpatialAudio(ActiveSource& source, const components::TransformComponent* transform)
{
    if (!transform)
        return;

    sf::Vector3f pos(transform->position.x, transform->position.y, transform->position.z);
    source.sound.setPosition(pos);

    float dx = pos.x - m_listenerPosition.x;
    float dy = pos.y - m_listenerPosition.y;
    float dz = pos.z - m_listenerPosition.z;
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    float attenuation = 1.0f / (1.0f + 0.1f * distance);
    source.sound.setVolume(source.sound.getVolume() * attenuation);
}

void AudioSystem::ReclaimStoppedSources()
{
    for (uint32_t i = 0; i < m_sources.size(); ++i)
    {
        if (m_sources[i].valid && m_sources[i].sound.getStatus() == sf::Sound::Stopped)
        {
            m_sources[i].valid = false;
            m_freeSlots.push(i);
        }
    }
}

AudioSystem::ActiveSource* AudioSystem::AcquireSource()
{
    if (m_freeSlots.empty())
        return nullptr;

    uint32_t slot = m_freeSlots.front();
    m_freeSlots.pop();
    return &m_sources[slot];
}

void AudioSystem::StopSource(ActiveSource& source)
{
    source.sound.stop();
    source.valid = false;
}

} // namespace systems
} // namespace engine

