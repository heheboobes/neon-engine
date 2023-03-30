#ifndef ENGINE_CORE_TIME_H
#define ENGINE_CORE_TIME_H

#include <cstdint>
#include <chrono>

namespace engine {
namespace core {

class Time {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    static Time& Instance();

    void Update();

    float GetDeltaTime() const { return m_deltaTime; }
    float GetFixedDeltaTime() const { return m_fixedDeltaTime; }
    float GetTimeSinceStartup() const { return m_totalTime; }
    uint64_t GetFrameCount() const { return m_frameCount; }
    float GetFPS() const { return m_fps; }
    float GetFrameTimeMs() const { return m_deltaTime * 1000.0f; }
    float GetTimeScale() const { return m_timeScale; }
    void SetTimeScale(float scale) { m_timeScale = scale; }
    float GetUnscaledDeltaTime() const { return m_unscaledDeltaTime; }
    float GetRealtimeSinceStartup() const { return m_realtimeSinceStartup; }

    void SetFixedDeltaTime(float dt) { m_fixedDeltaTime = dt; }
    void Reset();

private:
    Time();
    ~Time() = default;
    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;

    void CalculateFPS();

    TimePoint m_startTime;
    TimePoint m_lastFrameTime;
    float m_deltaTime = 0.0f;
    float m_unscaledDeltaTime = 0.0f;
    float m_fixedDeltaTime = 1.0f / 60.0f;
    float m_totalTime = 0.0f;
    float m_realtimeSinceStartup = 0.0f;
    float m_timeScale = 1.0f;
    float m_fps = 0.0f;
    uint64_t m_frameCount = 0;
    float m_fpsAccumulator = 0.0f;
    uint32_t m_fpsFrameCount = 0;
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_TIME_H
// v2: Added frame time history
