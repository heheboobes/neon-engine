#include "Time.h"
#include <algorithm>

namespace engine {
namespace core {

Time::Time()
    : m_startTime(Clock::now())
    , m_lastFrameTime(Clock::now())
{
}

Time& Time::Instance()
{
    static Time instance;
    return instance;
}

void Time::Update()
{
    TimePoint now = Clock::now();
    m_frameCount++;

    Duration frameDuration = now - m_lastFrameTime;
    m_lastFrameTime = now;

    Duration totalDuration = now - m_startTime;

    float rawDelta = std::chrono::duration<float>(frameDuration).count();
    m_unscaledDeltaTime = rawDelta;
    m_deltaTime = rawDelta * m_timeScale;
    m_totalTime += m_deltaTime;
    m_realtimeSinceStartup = std::chrono::duration<float>(totalDuration).count();

    m_deltaTime = std::min(m_deltaTime, 0.1f);

    CalculateFPS();
}

void Time::CalculateFPS()
{
    m_fpsAccumulator += m_unscaledDeltaTime;
    m_fpsFrameCount++;

    if (m_fpsAccumulator >= 0.5f)
    {
        m_fps = static_cast<float>(m_fpsFrameCount) / m_fpsAccumulator;
        m_fpsAccumulator = 0.0f;
        m_fpsFrameCount = 0;
    }
}

void Time::Reset()
{
    m_startTime = Clock::now();
    m_lastFrameTime = Clock::now();
    m_deltaTime = 0.0f;
    m_unscaledDeltaTime = 0.0f;
    m_totalTime = 0.0f;
    m_realtimeSinceStartup = 0.0f;
    m_frameCount = 0;
    m_fpsAccumulator = 0.0f;
    m_fpsFrameCount = 0;
    m_fps = 0.0f;
}

} // namespace core
} // namespace engine
