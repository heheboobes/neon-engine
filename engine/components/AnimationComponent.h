#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include "../../engine/ecs/Component.h"
#include <vector>
#include <string>

namespace engine {
namespace components {

enum class LoopMode {
    None,
    Loop,
    PingPong
};

struct AnimationFrame {
    sf::IntRect rect;
    float duration;
};

class AnimationComponent : public ecs::Component<AnimationComponent> {
public:
    std::vector<AnimationFrame> frames;
    std::string animationName;

    LoopMode loopMode;
    float playbackSpeed;
    int currentFrameIndex;
    bool playing;
    bool finished;

    AnimationComponent();

    void AddFrame(const sf::IntRect& rect, float duration);
    void SetFrames(const std::vector<AnimationFrame>& newFrames);
    void ClearFrames();

    void Play(const std::string& name, LoopMode mode = LoopMode::Loop);
    void Stop();
    void Pause();
    void Resume();

    void SetFrame(int index);
    int GetFrameCount() const;
    AnimationFrame GetCurrentFrame() const;

    void Update(float deltaTime);

    bool IsPlaying() const;
    bool IsFinished() const;

    sf::IntRect GetCurrentTextureRect() const;

private:
    sf::Clock m_timer;
    float m_accumulator;
    bool m_forward;
};

} // namespace components
} // namespace engine
