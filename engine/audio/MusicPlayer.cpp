#include "MusicPlayer.h"
#include <algorithm>
#include <random>
#include <chrono>

namespace engine::audio {

MusicPlayer::MusicPlayer() = default;

MusicPlayer::~MusicPlayer() {
    stop();
}

bool MusicPlayer::load(const std::string& id, const std::string& filepath) {
    sf::Music test;
    if (!test.openFromFile(filepath)) {
        return false;
    }
    m_trackPaths[id] = filepath;
    return true;
}

void MusicPlayer::play(const std::string& id) {
    if (!id.empty()) {
        auto it = m_trackPaths.find(id);
        if (it == m_trackPaths.end()) return;
        m_currentId = id;
        if (!m_music.openFromFile(it->second)) return;

        for (int i = 0; i < static_cast<int>(m_playlist.size()); ++i) {
            if (m_playlist[i].id == id) {
                m_currentIndex = i;
                break;
            }
        }
    }

    m_music.setVolume(m_volume);
    m_music.setPitch(m_pitch);
    m_music.setLoop(m_loopMode == LoopMode::Single);
    m_music.play();
}

void MusicPlayer::stop() {
    m_music.stop();
    m_fade.active = false;
}

void MusicPlayer::pause() {
    m_music.pause();
}

void MusicPlayer::resume() {
    m_music.play();
}

void MusicPlayer::setVolume(float volume) {
    m_volume = std::clamp(volume, 0.0f, 100.0f);
    applyVolume();
}

float MusicPlayer::getVolume() const {
    return m_volume;
}

void MusicPlayer::setPitch(float pitch) {
    m_pitch = std::max(0.01f, pitch);
    m_music.setPitch(m_pitch);
}

float MusicPlayer::getPitch() const {
    return m_pitch;
}

void MusicPlayer::setLoopMode(LoopMode mode) {
    m_loopMode = mode;
    m_music.setLoop(mode == LoopMode::Single);
}

LoopMode MusicPlayer::getLoopMode() const {
    return m_loopMode;
}

void MusicPlayer::setPlaylist(const std::vector<PlaylistEntry>& entries) {
    m_playlist = entries;
    m_currentIndex = m_playlist.empty() ? -1 : 0;
    m_shuffleHistory.clear();
    if (m_loopMode == LoopMode::Shuffle) {
        m_shuffleHistory.push_back(m_currentIndex);
    }
}

void MusicPlayer::addToPlaylist(const PlaylistEntry& entry) {
    m_playlist.push_back(entry);
}

void MusicPlayer::clearPlaylist() {
    m_playlist.clear();
    m_currentIndex = -1;
    m_shuffleHistory.clear();
}

void MusicPlayer::next() {
    selectNextTrack(true);
}

void MusicPlayer::previous() {
    selectNextTrack(false);
}

void MusicPlayer::crossfade(const std::string& nextId, float duration) {
    fadeOut(duration);
    auto it = m_trackPaths.find(nextId);
    if (it != m_trackPaths.end()) {
        m_currentId = nextId;
        if (m_music.openFromFile(it->second)) {
            m_music.setVolume(0.0f);
            m_music.setPitch(m_pitch);
            m_music.play();
            fadeIn(duration);
        }
    }
}

void MusicPlayer::fadeIn(float duration) {
    m_fade = {FadeDirection::In, duration, 0.0f, 0.0f, m_volume, true};
    m_music.setVolume(0.0f);
}

void MusicPlayer::fadeOut(float duration) {
    m_fade = {FadeDirection::Out, duration, 0.0f, m_volume, 0.0f, true};
}

bool MusicPlayer::isPlaying() const {
    return m_music.getStatus() == sf::SoundSource::Playing;
}

bool MusicPlayer::isPaused() const {
    return m_music.getStatus() == sf::SoundSource::Paused;
}

sf::Time MusicPlayer::getPlayingOffset() const {
    return m_music.getPlayingOffset();
}

sf::Time MusicPlayer::getDuration() const {
    return m_music.getDuration();
}

std::string MusicPlayer::getCurrentId() const {
    return m_currentId;
}

void MusicPlayer::update(float dt) {
    if (m_fade.active) {
        m_fade.elapsed += dt;
        float t = std::min(m_fade.elapsed / m_fade.duration, 1.0f);
        float vol = m_fade.startVolume + (m_fade.targetVolume - m_fade.startVolume) * t;
        m_music.setVolume(vol);
        if (t >= 1.0f) {
            m_fade.active = false;
            m_music.setVolume(m_fade.targetVolume);
        }
    }

    if (m_music.getStatus() == sf::SoundSource::Stopped && !m_fade.active) {
        if (m_onComplete) {
            m_onComplete(m_currentId);
        }
        if (m_loopMode == LoopMode::Single) {
            m_music.play();
        } else if (m_loopMode == LoopMode::Playlist || m_loopMode == LoopMode::Shuffle) {
            selectNextTrack(true);
            play();
        }
    }
}

void MusicPlayer::setOnTrackComplete(TrackCompletionCallback callback) {
    m_onComplete = std::move(callback);
}

void MusicPlayer::selectNextTrack(bool forward) {
    if (m_playlist.empty()) return;

    if (m_loopMode == LoopMode::Shuffle) {
        m_currentIndex = pickShuffleIndex();
        m_shuffleHistory.push_back(m_currentIndex);
    } else {
        if (forward) {
            m_currentIndex = (m_currentIndex + 1) % m_playlist.size();
        } else {
            m_currentIndex = (m_currentIndex - 1 + m_playlist.size()) % m_playlist.size();
        }
    }

    if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_playlist.size())) {
        m_currentId = m_playlist[m_currentIndex].id;
        auto it = m_trackPaths.find(m_currentId);
        if (it != m_trackPaths.end()) {
            m_music.openFromFile(it->second);
        }
    }
}

int MusicPlayer::pickShuffleIndex() const {
    if (m_playlist.size() <= 1) return 0;
    unsigned seed = static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(m_playlist.size()) - 1);
    int idx;
    do {
        idx = dist(rng);
    } while (idx == m_currentIndex && m_playlist.size() > 1);
    return idx;
}

void MusicPlayer::applyVolume() {
    float vol = m_fade.active ? m_fade.startVolume : m_volume;
    m_music.setVolume(vol);
}

} // namespace engine::audio
