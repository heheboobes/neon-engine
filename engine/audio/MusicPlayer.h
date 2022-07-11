#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <functional>

namespace engine::audio {

enum class LoopMode {
    None,
    Single,
    Playlist,
    Shuffle
};

enum class FadeDirection {
    In,
    Out
};

struct PlaylistEntry {
    std::string id;
    std::string filepath;
    float volumeBoost = 0.0f;
};

struct FadeState {
    FadeDirection direction = FadeDirection::In;
    float duration = 0.0f;
    float elapsed = 0.0f;
    float startVolume = 0.0f;
    float targetVolume = 100.0f;
    bool active = false;
};

class MusicPlayer {
public:
    MusicPlayer();
    ~MusicPlayer();

    bool load(const std::string& id, const std::string& filepath);
    void play(const std::string& id = "");
    void stop();
    void pause();
    void resume();

    void setVolume(float volume);
    float getVolume() const;
    void setPitch(float pitch);
    float getPitch() const;

    void setLoopMode(LoopMode mode);
    LoopMode getLoopMode() const;

    void setPlaylist(const std::vector<PlaylistEntry>& entries);
    void addToPlaylist(const PlaylistEntry& entry);
    void clearPlaylist();
    void next();
    void previous();

    void crossfade(const std::string& nextId, float duration = 2.0f);
    void fadeIn(float duration = 2.0f);
    void fadeOut(float duration = 2.0f);

    bool isPlaying() const;
    bool isPaused() const;
    sf::Time getPlayingOffset() const;
    sf::Time getDuration() const;
    std::string getCurrentId() const;

    void update(float dt);

    using TrackCompletionCallback = std::function<void(const std::string&)>;
    void setOnTrackComplete(TrackCompletionCallback callback);

private:
    void selectNextTrack(bool forward);
    int pickShuffleIndex() const;
    void applyVolume();

    sf::Music m_music;
    std::unordered_map<std::string, std::string> m_trackPaths;
    std::vector<PlaylistEntry> m_playlist;
    int m_currentIndex = -1;
    std::string m_currentId;
    LoopMode m_loopMode = LoopMode::None;
    float m_volume = 100.0f;
    float m_pitch = 1.0f;
    FadeState m_fade;
    std::vector<int> m_shuffleHistory;
    TrackCompletionCallback m_onComplete;
};

} // namespace engine::audio
