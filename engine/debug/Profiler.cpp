#include "Profiler.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace engine::debug {

Profiler& Profiler::instance() {
    static Profiler s_instance;
    return s_instance;
}

void Profiler::beginFrame() {
    m_currentFrame.samples.clear();
    m_currentFrame.frameIndex = m_previousFrame.frameIndex + 1;

    auto now = Clock::now();
    if (m_previousFrame.frameIndex > 0) {
        auto diff = now - m_previousFrame.endTime;
        m_currentFrame.deltaTimeMs = std::chrono::duration<double, std::milli>(diff).count();
    }
    m_currentFrame.frameTimeMs = 0.0;
}

void Profiler::endFrame() {
    auto end = Clock::now();
    auto start = end - std::chrono::microseconds(static_cast<long long>(m_currentFrame.frameTimeMs * 1000.0));

    m_currentFrame.frameTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    m_previousFrame = m_currentFrame;
    m_previousFrame.endTime = end;

    m_frameHistory.push_back(m_previousFrame);
    if (m_frameHistory.size() > m_maxHistory) {
        m_frameHistory.erase(m_frameHistory.begin());
    }
}

void Profiler::beginSample(const char* name) {
    ActiveSample sample;
    sample.name = name;
    sample.start = Clock::now();
    sample.depth = m_activeStack.size();
    m_activeStack.push_back(sample);
}

void Profiler::endSample() {
    if (m_activeStack.empty()) return;

    auto end = Clock::now();
    auto& sample = m_activeStack.back();
    double elapsed = std::chrono::duration<double, std::milli>(end - sample.start).count();

    auto it = std::find_if(m_currentFrame.samples.begin(), m_currentFrame.samples.end(),
        [&](const ProfileSample& ps) { return ps.name == sample.name; });

    if (it != m_currentFrame.samples.end()) {
        it->elapsedMs += elapsed;
        it->callCount++;
        it->minMs = std::min(it->minMs, elapsed);
        it->maxMs = std::max(it->maxMs, elapsed);
        it->averageMs = it->elapsedMs / static_cast<double>(it->callCount);
    } else {
        ProfileSample ps;
        ps.name = sample.name;
        ps.elapsedMs = elapsed;
        ps.minMs = elapsed;
        ps.maxMs = elapsed;
        ps.averageMs = elapsed;
        ps.callCount = 1;
        ps.frameCount = 1;
        m_currentFrame.samples.push_back(ps);
    }

    m_activeStack.pop_back();
}

const FrameData& Profiler::getCurrentFrame() const {
    return m_currentFrame;
}

const FrameData& Profiler::getPreviousFrame() const {
    return m_previousFrame;
}

const std::vector<FrameData>& Profiler::getFrameHistory() const {
    return m_frameHistory;
}

double Profiler::getAverageFrameTime() const {
    if (m_frameHistory.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& fd : m_frameHistory) {
        sum += fd.frameTimeMs;
    }
    return sum / static_cast<double>(m_frameHistory.size());
}

double Profiler::getMinFrameTime() const {
    if (m_frameHistory.empty()) return 0.0;
    double min = std::numeric_limits<double>::max();
    for (const auto& fd : m_frameHistory) {
        if (fd.frameTimeMs < min) min = fd.frameTimeMs;
    }
    return min;
}

double Profiler::getMaxFrameTime() const {
    if (m_frameHistory.empty()) return 0.0;
    double max = 0.0;
    for (const auto& fd : m_frameHistory) {
        if (fd.frameTimeMs > max) max = fd.frameTimeMs;
    }
    return max;
}

double Profiler::getFPS() const {
    double avg = getAverageFrameTime();
    return (avg > 0.0) ? 1000.0 / avg : 0.0;
}

void Profiler::setHistorySize(size_t frames) {
    m_maxHistory = std::max<size_t>(frames, 1);
    while (m_frameHistory.size() > m_maxHistory) {
        m_frameHistory.erase(m_frameHistory.begin());
    }
}

size_t Profiler::getHistorySize() const {
    return m_maxHistory;
}

void Profiler::reset() {
    m_currentFrame = FrameData{};
    m_previousFrame = FrameData{};
    m_frameHistory.clear();
    m_activeStack.clear();
}

std::string Profiler::generateReport() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "=== Profiler Report ===\n";
    oss << "FPS: " << getFPS() << "\n";
    oss << "Frame Time: avg=" << getAverageFrameTime()
        << "ms min=" << getMinFrameTime()
        << "ms max=" << getMaxFrameTime() << "ms\n";

    if (!m_previousFrame.samples.empty()) {
        oss << "\n--- Last Frame Samples ---\n";
        for (const auto& s : m_previousFrame.samples) {
            oss << "  " << s.name << ": " << s.elapsedMs << "ms"
                << " (min=" << s.minMs << " max=" << s.maxMs
                << " avg=" << s.averageMs << " calls=" << s.callCount << ")\n";
        }
    }

    return oss.str();
}

std::string Profiler::generateHierarchicalReport() const {
    std::ostringstream oss;
    oss << "=== Hierarchical Profile ===\n";
    oss << "Frame " << m_previousFrame.frameIndex << ": "
        << m_previousFrame.frameTimeMs << "ms\n";
    for (const auto& s : m_previousFrame.samples) {
        double pct = m_previousFrame.frameTimeMs > 0.0
            ? (s.elapsedMs / m_previousFrame.frameTimeMs) * 100.0
            : 0.0;
        oss << "  " << s.name << ": " << s.elapsedMs << "ms ("
            << std::setprecision(1) << pct << "%)\n";
    }
    return oss.str();
}

} // namespace engine::debug
