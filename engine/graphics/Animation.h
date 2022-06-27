#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

namespace engine {
namespace graphics {

struct AnimationFrame {
    sf::IntRect subRect;
    sf::Vector2f origin;
    float duration;
    std::string eventName;
    int frameIndex;

    AnimationFrame()
        : duration(0.1f)
        , frameIndex(0) {}
};

struct AnimationClip {
    std::string name;
    std::vector<AnimationFrame> frames;
    bool looping;
    bool reversed;
    float totalDuration;
    std::string nextClip;

    AnimationClip()
        : looping(true)
        , reversed(false)
        , totalDuration(0.f) {}
};

enum class AnimationState : uint8_t {
    Stopped,
    Playing,
    Paused,
    Finished
};

enum class AnimationWrapMode : uint8_t {
    Once,
    Loop,
    PingPong,
    ClampForever
};

class Animation {
public:
    Animation();
    explicit Animation(const AnimationClip& clip);
    ~Animation() = default;

    void play();
    void play(const std::string& clipName);
    void stop();
    void pause();
    void resume();
    void reset();

    void update(float dt);
    void setTime(float time);
    void setFrame(int frameIndex);
    void setSpeed(float speed);

    void addClip(const AnimationClip& clip);
    bool hasClip(const std::string& name) const;
    const AnimationClip* getClip(const std::string& name) const;
    bool setClip(const std::string& name);

    const AnimationFrame* getCurrentFrame() const;
    int getCurrentFrameIndex() const;
    float getCurrentTime() const;
    float getTotalDuration() const;
    float getProgress() const;
    std::size_t getFrameCount() const;

    AnimationState getState() const;
    AnimationWrapMode getWrapMode() const;
    void setWrapMode(AnimationWrapMode mode);

    void setLooping(bool looping);
    bool isLooping() const;

    sf::IntRect getCurrentSubRect() const;
    sf::Vector2f getCurrentOrigin() const;
    void applyTo(sf::Sprite& sprite) const;

    void onFrameEvent(std::function<void(const std::string&, int)> callback);
    void onFinished(std::function<void()> callback);
    void onLooped(std::function<void()> callback);

    std::string getCurrentClipName() const;
    std::vector<std::string> getClipNames() const;

    bool isPlaying() const;
    bool isPaused() const;
    bool isFinished() const;

    void setFrameOffset(int offset);
    int getFrameOffset() const;

    static AnimationFrame createFrame(const sf::IntRect& rect, float duration);
    static AnimationClip createSimpleClip(const std::string& name,
                                          const sf::Vector2i& frameSize,
                                          int columns, int totalFrames,
                                          float frameDuration,
                                          const sf::Vector2i& startPos = {0, 0});

private:
    AnimationClip* findClip(const std::string& name);
    int getNextFrameIndex() const;

    std::vector<AnimationClip> m_clips;
    std::size_t m_currentClip;
    int m_currentFrame;
    float m_currentTime;
    float m_speed;

    AnimationState m_state;
    AnimationWrapMode m_wrapMode;
    int m_frameOffset;

    std::function<void(const std::string&, int)> m_frameEventCallback;
    std::function<void()> m_finishedCallback;
    std::function<void()> m_loopedCallback;
};

} // namespace graphics
} // namespace engine
