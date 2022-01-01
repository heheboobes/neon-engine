#include "Animation.h"
#include <cassert>
#include <algorithm>
#include <cmath>

namespace engine {
namespace graphics {

Animation::Animation()
    : m_currentClip(0)
    , m_currentFrame(0)
    , m_currentTime(0.f)
    , m_speed(1.f)
    , m_state(AnimationState::Stopped)
    , m_wrapMode(AnimationWrapMode::Loop)
    , m_frameOffset(0) {}

Animation::Animation(const AnimationClip& clip)
    : m_currentClip(0)
    , m_currentFrame(0)
    , m_currentTime(0.f)
    , m_speed(1.f)
    , m_state(AnimationState::Stopped)
    , m_wrapMode(clip.looping ? AnimationWrapMode::Loop : AnimationWrapMode::Once)
    , m_frameOffset(0) {
    m_clips.push_back(clip);
}

void Animation::play() {
    if (m_clips.empty()) return;
    m_state = AnimationState::Playing;
    if (m_currentTime >= getTotalDuration() && !m_clips[m_currentClip].looping) {
        m_currentTime = 0.f;
        m_currentFrame = 0;
    }
}

void Animation::play(const std::string& clipName) {
    if (setClip(clipName)) {
        m_state = AnimationState::Playing;
    }
}

void Animation::stop() {
    m_state = AnimationState::Stopped;
    m_currentTime = 0.f;
    m_currentFrame = 0;
}

void Animation::pause() {
    if (m_state == AnimationState::Playing) {
        m_state = AnimationState::Paused;
    }
}

void Animation::resume() {
    if (m_state == AnimationState::Paused) {
        m_state = AnimationState::Playing;
    }
}

void Animation::reset() {
    m_currentTime = 0.f;
    m_currentFrame = 0;
    m_state = AnimationState::Stopped;
}

void Animation::update(float dt) {
    if (m_state != AnimationState::Playing || m_clips.empty()) return;

    if (m_currentClip >= m_clips.size()) return;

    const AnimationClip& clip = m_clips[m_currentClip];
    if (clip.frames.empty()) return;

    float adjustedDt = dt * m_speed;
    m_currentTime += adjustedDt;

    float totalDuration = clip.totalDuration;
    if (totalDuration <= 0.f) return;

    if (m_currentTime >= totalDuration) {
        if (clip.looping) {
            m_currentTime = std::fmod(m_currentTime, totalDuration);
            if (m_loopedCallback) m_loopedCallback();
        } else {
            m_currentTime = totalDuration;
            m_currentFrame = static_cast<int>(clip.frames.size()) - 1;
            m_state = AnimationState::Finished;
            if (m_finishedCallback) m_finishedCallback();

            if (!clip.nextClip.empty()) {
                setClip(clip.nextClip);
                m_state = AnimationState::Playing;
            }
            return;
        }
    }

    float accumulated = 0.f;
    for (std::size_t i = 0; i < clip.frames.size(); ++i) {
        int idx = clip.reversed ? static_cast<int>(clip.frames.size() - 1 - i) : static_cast<int>(i);
        accumulated += clip.frames[idx].duration;
        if (m_currentTime <= accumulated) {
            if (static_cast<int>(i) != m_currentFrame) {
                int oldFrame = m_currentFrame;
                m_currentFrame = idx;
                if (m_frameEventCallback && !clip.frames[idx].eventName.empty()) {
                    m_frameEventCallback(clip.frames[idx].eventName, idx);
                }
            }
            return;
        }
    }
}

void Animation::setTime(float time) {
    m_currentTime = std::max(0.f, time);
    m_currentFrame = 0;

    if (m_clips.empty() || m_currentClip >= m_clips.size()) return;
    const auto& clip = m_clips[m_currentClip];
    float accumulated = 0.f;
    for (std::size_t i = 0; i < clip.frames.size(); ++i) {
        accumulated += clip.frames[i].duration;
        if (m_currentTime <= accumulated) {
            m_currentFrame = static_cast<int>(i);
            return;
        }
    }
}

void Animation::setFrame(int frameIndex) {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return;
    const auto& clip = m_clips[m_currentClip];
    m_currentFrame = std::clamp(frameIndex, 0, static_cast<int>(clip.frames.size()) - 1);
    m_currentTime = 0.f;
    for (int i = 0; i < m_currentFrame; ++i) {
        m_currentTime += clip.frames[i].duration;
    }
}

void Animation::setSpeed(float speed) {
    m_speed = std::max(0.f, speed);
}

void Animation::addClip(const AnimationClip& clip) {
    m_clips.push_back(clip);
    if (m_clips.size() == 1) {
        m_currentClip = 0;
    }
}

bool Animation::hasClip(const std::string& name) const {
    for (const auto& clip : m_clips) {
        if (clip.name == name) return true;
    }
    return false;
}

const AnimationClip* Animation::getClip(const std::string& name) const {
    for (const auto& clip : m_clips) {
        if (clip.name == name) return &clip;
    }
    return nullptr;
}

bool Animation::setClip(const std::string& name) {
    for (std::size_t i = 0; i < m_clips.size(); ++i) {
        if (m_clips[i].name == name) {
            m_currentClip = i;
            m_currentFrame = 0;
            m_currentTime = 0.f;
            m_wrapMode = m_clips[i].looping ? AnimationWrapMode::Loop : AnimationWrapMode::Once;
            return true;
        }
    }
    return false;
}

const AnimationFrame* Animation::getCurrentFrame() const {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return nullptr;
    const auto& clip = m_clips[m_currentClip];
    if (clip.frames.empty()) return nullptr;
    int idx = std::clamp(m_currentFrame, 0, static_cast<int>(clip.frames.size()) - 1);
    return &clip.frames[idx];
}

int Animation::getCurrentFrameIndex() const {
    return m_currentFrame;
}

float Animation::getCurrentTime() const {
    return m_currentTime;
}

float Animation::getTotalDuration() const {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return 0.f;
    return m_clips[m_currentClip].totalDuration;
}

float Animation::getProgress() const {
    float total = getTotalDuration();
    return (total > 0.f) ? m_currentTime / total : 0.f;
}

std::size_t Animation::getFrameCount() const {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return 0;
    return m_clips[m_currentClip].frames.size();
}

AnimationState Animation::getState() const {
    return m_state;
}

AnimationWrapMode Animation::getWrapMode() const {
    return m_wrapMode;
}

void Animation::setWrapMode(AnimationWrapMode mode) {
    m_wrapMode = mode;
    if (!m_clips.empty() && m_currentClip < m_clips.size()) {
        m_clips[m_currentClip].looping = (mode == AnimationWrapMode::Loop || mode == AnimationWrapMode::PingPong);
    }
}

void Animation::setLooping(bool looping) {
    if (!m_clips.empty() && m_currentClip < m_clips.size()) {
        m_clips[m_currentClip].looping = looping;
    }
}

bool Animation::isLooping() const {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return false;
    return m_clips[m_currentClip].looping;
}

sf::IntRect Animation::getCurrentSubRect() const {
    const AnimationFrame* frame = getCurrentFrame();
    if (frame) {
        sf::IntRect r = frame->subRect;
        if (m_frameOffset != 0) {
            r.left += m_frameOffset * r.width;
        }
        return r;
    }
    return sf::IntRect();
}

sf::Vector2f Animation::getCurrentOrigin() const {
    const AnimationFrame* frame = getCurrentFrame();
    return frame ? frame->origin : sf::Vector2f();
}

void Animation::applyTo(sf::Sprite& sprite) const {
    const AnimationFrame* frame = getCurrentFrame();
    if (frame) {
        sf::IntRect r = frame->subRect;
        if (m_frameOffset != 0) {
            r.left += m_frameOffset * r.width;
        }
        sprite.setTextureRect(r);
        sprite.setOrigin(frame->origin);
    }
}

void Animation::onFrameEvent(std::function<void(const std::string&, int)> callback) {
    m_frameEventCallback = std::move(callback);
}

void Animation::onFinished(std::function<void()> callback) {
    m_finishedCallback = std::move(callback);
}

void Animation::onLooped(std::function<void()> callback) {
    m_loopedCallback = std::move(callback);
}

std::string Animation::getCurrentClipName() const {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return "";
    return m_clips[m_currentClip].name;
}

std::vector<std::string> Animation::getClipNames() const {
    std::vector<std::string> names;
    names.reserve(m_clips.size());
    for (const auto& clip : m_clips) {
        names.push_back(clip.name);
    }
    return names;
}

bool Animation::isPlaying() const {
    return m_state == AnimationState::Playing;
}

bool Animation::isPaused() const {
    return m_state == AnimationState::Paused;
}

bool Animation::isFinished() const {
    return m_state == AnimationState::Finished;
}

void Animation::setFrameOffset(int offset) {
    m_frameOffset = offset;
}

int Animation::getFrameOffset() const {
    return m_frameOffset;
}

AnimationFrame Animation::createFrame(const sf::IntRect& rect, float duration) {
    AnimationFrame frame;
    frame.subRect = rect;
    frame.duration = duration;
    return frame;
}

AnimationClip Animation::createSimpleClip(const std::string& name,
                                          const sf::Vector2i& frameSize,
                                          int columns, int totalFrames,
                                          float frameDuration,
                                          const sf::Vector2i& startPos) {
    AnimationClip clip;
    clip.name = name;
    clip.looping = true;
    clip.frames.reserve(totalFrames);

    for (int i = 0; i < totalFrames; ++i) {
        int col = i % columns;
        int row = i / columns;
        AnimationFrame frame;
        frame.subRect = sf::IntRect(
            startPos.x + col * frameSize.x,
            startPos.y + row * frameSize.y,
            frameSize.x, frameSize.y);
        frame.duration = frameDuration;
        frame.frameIndex = i;
        clip.frames.push_back(frame);
        clip.totalDuration += frameDuration;
    }

    return clip;
}

AnimationClip* Animation::findClip(const std::string& name) {
    for (auto& clip : m_clips) {
        if (clip.name == name) return &clip;
    }
    return nullptr;
}

int Animation::getNextFrameIndex() const {
    if (m_clips.empty() || m_currentClip >= m_clips.size()) return 0;
    const auto& clip = m_clips[m_currentClip];
    if (clip.frames.empty()) return 0;
    return (m_currentFrame + 1) % static_cast<int>(clip.frames.size());
}

} // namespace graphics
} // namespace engine

