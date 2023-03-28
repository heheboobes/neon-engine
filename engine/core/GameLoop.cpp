#include "GameLoop.h"
#include "Time.h"
#include <algorithm>
#include <chrono>
#include <thread>

namespace engine {
namespace core {

GameLoop::GameLoop()
{
}

void GameLoop::Run()
{
    m_running = true;
    Time& time = Time::Instance();
    time.Reset();

    while (m_running)
    {
        Tick();
    }
}

void GameLoop::Stop()
{
    m_running = false;
}

void GameLoop::Tick()
{
    Time& time = Time::Instance();
    time.Update();

    m_frameTime = time.GetUnscaledDeltaTime();
    m_frameTime = std::min(m_frameTime, 0.1f);

    m_accumulator += m_frameTime;

    auto fixedStart = std::chrono::high_resolution_clock::now();
    uint32_t subSteps = 0;

    while (m_accumulator >= m_fixedTimestep && subSteps < m_maxSubSteps)
    {
        if (m_fixedUpdateFunc)
            m_fixedUpdateFunc(m_fixedTimestep);

        m_accumulator -= m_fixedTimestep;
        subSteps++;
        m_stats.totalFixedUpdates++;
    }

    auto fixedEnd = std::chrono::high_resolution_clock::now();
    m_stats.fixedUpdateTimeMs = std::chrono::duration<float, std::milli>(fixedEnd - fixedStart).count();

    float alpha = m_accumulator / m_fixedTimestep;

    auto updateStart = std::chrono::high_resolution_clock::now();
    if (m_updateFunc)
        m_updateFunc(time.GetDeltaTime());
    auto updateEnd = std::chrono::high_resolution_clock::now();
    m_stats.updateTimeMs = std::chrono::duration<float, std::milli>(updateEnd - updateStart).count();

    auto renderStart = std::chrono::high_resolution_clock::now();
    if (m_renderFunc)
        m_renderFunc(alpha);
    auto renderEnd = std::chrono::high_resolution_clock::now();
    m_stats.renderTimeMs = std::chrono::duration<float, std::milli>(renderEnd - renderStart).count();

    m_stats.fps = time.GetFPS();
    m_stats.frameTimeMs = m_frameTime * 1000.0f;
    m_stats.interpolationFactor = alpha;
    m_stats.totalFrames++;

    if (m_targetFPS > 0)
    {
        float targetFrameTime = 1.0f / static_cast<float>(m_targetFPS);
        float elapsed = std::chrono::duration<float>(
            std::chrono::high_resolution_clock::now() - renderEnd).count();
        float sleepTime = (targetFrameTime - elapsed) * 1000.0f;
        if (sleepTime > 1.0f)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(sleepTime)));
        }
    }
}

} // namespace core
} // namespace engine
// v2: Fixed sub-step cap
