#pragma once

#include <chrono>
#include <functional>

namespace engine::utils {

enum class TimerMode {
    Countdown,
    Stopwatch
};

enum class TimerState {
    Stopped,
    Running,
    Paused,
    Expired
};

class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<double>;
    using Callback = std::function<void()>;

    explicit Timer(TimerMode mode = TimerMode::Countdown);
    ~Timer() = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = default;
    Timer& operator=(Timer&&) = default;

    void start();
    void stop();
    void reset();
    void pause();
    void resume();

    void setDuration(double seconds);
    double getDuration() const;

    double getElapsed() const;
    double getRemaining() const;

    void setLoop(bool loop) { m_loop = loop; }
    bool isLooping() const { return m_loop; }

    void setCallback(Callback cb) { m_callback = std::move(cb); }

    TimerState getState() const { return m_state; }
    TimerMode getMode() const { return m_mode; }

    void update();

    bool isExpired() const { return m_state == TimerState::Expired; }
    bool isRunning() const { return m_state == TimerState::Running; }

    double getProgress() const;

private:
    TimerMode m_mode;
    TimerState m_state{ TimerState::Stopped };
    bool m_loop{ false };
    double m_duration{ 1.0 };
    Clock::time_point m_startTime;
    double m_accumulated{ 0.0 };
    Callback m_callback;
};

} // namespace engine::utils
