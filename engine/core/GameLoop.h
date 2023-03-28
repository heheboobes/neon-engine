#ifndef ENGINE_CORE_GAME_LOOP_H
#define ENGINE_CORE_GAME_LOOP_H

#include <functional>
#include <cstdint>

namespace engine {
namespace core {

struct GameLoopStats {
    float fps = 0.0f;
    float frameTimeMs = 0.0f;
    float fixedUpdateTimeMs = 0.0f;
    float updateTimeMs = 0.0f;
    float renderTimeMs = 0.0f;
    float interpolationFactor = 0.0f;
    uint64_t totalFixedUpdates = 0;
    uint64_t totalFrames = 0;
};

class GameLoop {
public:
    using FixedUpdateFunc = std::function<void(float fixedDt)>;
    using UpdateFunc = std::function<void(float deltaTime)>;
    using RenderFunc = std::function<void(float alpha)>;

    GameLoop();
    ~GameLoop() = default;

    void SetFixedUpdateCallback(FixedUpdateFunc func) { m_fixedUpdateFunc = func; }
    void SetUpdateCallback(UpdateFunc func) { m_updateFunc = func; }
    void SetRenderCallback(RenderFunc func) { m_renderFunc = func; }

    void SetTargetFPS(uint32_t fps) { m_targetFPS = fps; }
    void SetFixedTimestep(float dt) { m_fixedTimestep = dt; }
    void SetMaxSubSteps(uint32_t steps) { m_maxSubSteps = steps; }

    void Run();
    void Stop();
    bool IsRunning() const { return m_running; }

    void Tick();

    const GameLoopStats& GetStats() const { return m_stats; }
    float GetAlpha() const { return m_accumulator / m_fixedTimestep; }

private:
    FixedUpdateFunc m_fixedUpdateFunc;
    UpdateFunc m_updateFunc;
    RenderFunc m_renderFunc;

    float m_fixedTimestep = 1.0f / 60.0f;
    float m_accumulator = 0.0f;
    float m_frameTime = 0.0f;
    uint32_t m_targetFPS = 144;
    uint32_t m_maxSubSteps = 5;
    volatile bool m_running = false;

    GameLoopStats m_stats;
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_GAME_LOOP_H
// v2: Improved interpolation alpha
