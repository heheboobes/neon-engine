#include "AnimationComponent.h"
#include <algorithm>

namespace engine {
namespace components {

AnimationComponent::AnimationComponent()
    : loopMode(LoopMode::None)
    , playbackSpeed(1.0f)
    , currentFrameIndex(0)
    , playing(false)
    , finished(false)
    , m_accumulator(0.0f)
    , m_forward(true)
{
}

void AnimationComponent::AddFrame(const sf::IntRect& rect, float duration)
{
    frames.push_back({rect, std::max(0.001f, duration)});
}

void AnimationComponent::SetFrames(const std::vector<AnimationFrame>& newFrames)
{
    frames = newFrames;
    currentFrameIndex = 0;
    m_accumulator = 0.0f;
    finished = false;
}

void AnimationComponent::ClearFrames()
{
    frames.clear();
    currentFrameIndex = 0;
    m_accumulator = 0.0f;
    playing = false;
    finished = false;
}

void AnimationComponent::Play(const std::string& name, LoopMode mode)
{
    animationName = name;
    loopMode = mode;
    currentFrameIndex = 0;
    m_accumulator = 0.0f;
    m_forward = true;
    playing = true;
    finished = false;
    m_timer.restart();
}

void AnimationComponent::Stop()
{
    playing = false;
    currentFrameIndex = 0;
    m_accumulator = 0.0f;
    finished = false;
}

void AnimationComponent::Pause()
{
    playing = false;
}

void AnimationComponent::Resume()
{
    if (!finished) {
        playing = true;
    }
}

void AnimationComponent::SetFrame(int index)
{
    if (index >= 0 && index < static_cast<int>(frames.size())) {
        currentFrameIndex = index;
        m_accumulator = 0.0f;
    }
}

int AnimationComponent::GetFrameCount() const
{
    return static_cast<int>(frames.size());
}

AnimationFrame AnimationComponent::GetCurrentFrame() const
{
    if (frames.empty()) return {sf::IntRect(), 0.0f};
    return frames[currentFrameIndex];
}

void AnimationComponent::Update(float deltaTime)
{
    if (!playing || frames.empty()) return;

    m_accumulator += deltaTime * playbackSpeed;
    float currentDuration = frames[currentFrameIndex].duration;

    if (m_accumulator >= currentDuration) {
        m_accumulator -= currentDuration;

        if (loopMode == LoopMode::PingPong) {
            if (m_forward) {
                if (currentFrameIndex + 1 >= static_cast<int>(frames.size())) {
                    m_forward = false;
                    currentFrameIndex--;
                } else {
                    currentFrameIndex++;
                }
            } else {
                if (currentFrameIndex - 1 < 0) {
                    m_forward = true;
                    currentFrameIndex++;
                } else {
                    currentFrameIndex--;
                }
            }
        } else {
            currentFrameIndex++;
            if (currentFrameIndex >= static_cast<int>(frames.size())) {
                if (loopMode == LoopMode::Loop) {
                    currentFrameIndex = 0;
                } else {
                    currentFrameIndex = static_cast<int>(frames.size()) - 1;
                    playing = false;
                    finished = true;
                }
            }
        }
    }
}

bool AnimationComponent::IsPlaying() const
{
    return playing;
}

bool AnimationComponent::IsFinished() const
{
    return finished;
}

sf::IntRect AnimationComponent::GetCurrentTextureRect() const
{
    if (frames.empty()) return sf::IntRect();
    return frames[currentFrameIndex].rect;
}

} // namespace components
} // namespace engine

