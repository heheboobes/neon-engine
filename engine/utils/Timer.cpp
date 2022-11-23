#include "Timer.h"
#include <algorithm>

namespace engine::utils {

Timer::Timer(TimerMode mode)
    : m_mode(mode)
{
}

void Timer::start()
{
    m_state = TimerState::Running;
    m_accumulated = 0.0;
    m_startTime = Clock::now();
}

void Timer::stop()
{
    m_state = TimerState::Stopped;
    m_accumulated = 0.0;
}

void Timer::reset()
{
    m_accumulated = 0.0;
    m_state = TimerState::Stopped;
    m_startTime = Clock::now();
}

void Timer::pause()
{
    if (m_state != TimerState::Running)
        return;
    auto now = Clock::now();
    Duration elapsed = now - m_startTime;
    m_accumulated += elapsed.count();
    m_state = TimerState::Paused;
}

void Timer::resume()
{
    if (m_state != TimerState::Paused)
        return;
    m_state = TimerState::Running;
    m_startTime = Clock::now();
}

void Timer::setDuration(double seconds)
{
    m_duration = std::max(0.0, seconds);
}

double Timer::getDuration() const
{
    return m_duration;
}

double Timer::getElapsed() const
{
    double elapsed = m_accumulated;
    if (m_state == TimerState::Running) {
        auto now = Clock::now();
        Duration d = now - m_startTime;
        elapsed += d.count();
    }
    if (m_mode == TimerMode::Countdown)
        return std::min(elapsed, m_duration);
    return elapsed;
}

double Timer::getRemaining() const
{
    if (m_mode == TimerMode::Stopwatch)
        return 0.0;
    return std::max(0.0, m_duration - getElapsed());
}

void Timer::update()
{
    if (m_state != TimerState::Running)
        return;

    double elapsed = getElapsed();

    if (m_mode == TimerMode::Countdown && elapsed >= m_duration) {
        if (m_loop) {
            m_accumulated = 0.0;
            m_startTime = Clock::now();
        } else {
            m_state = TimerState::Expired;
        }
        if (m_callback)
            m_callback();
    }
}

double Timer::getProgress() const
{
    if (m_duration <= 0.0)
        return 1.0;
    if (m_mode == TimerMode::Countdown)
        return std::min(1.0, getElapsed() / m_duration);
    return 0.0;
}

} // namespace engine::utils
