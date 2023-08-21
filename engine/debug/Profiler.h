#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <cstdint>

namespace engine::debug {

struct ProfileSample {
    std::string name;
    double elapsedMs{0.0};
    double minMs{0.0};
    double maxMs{0.0};
    double averageMs{0.0};
    uint64_t callCount{0};
    uint64_t frameCount{0};
};

struct FrameData {
    uint64_t frameIndex{0};
    double frameTimeMs{0.0};
    double deltaTimeMs{0.0};
    std::vector<ProfileSample> samples;
    std::chrono::high_resolution_clock::time_point endTime;
};

class Profiler {
public:
    static Profiler& instance();

    void beginFrame();
    void endFrame();

    void beginSample(const char* name);
    void endSample();

    const FrameData& getCurrentFrame() const;
    const FrameData& getPreviousFrame() const;
    const std::vector<FrameData>& getFrameHistory() const;

    double getAverageFrameTime() const;
    double getMinFrameTime() const;
    double getMaxFrameTime() const;
    double getFPS() const;

    void setHistorySize(size_t frames);
    size_t getHistorySize() const;

    void reset();

    std::string generateReport() const;
    std::string generateHierarchicalReport() const;

private:
    Profiler() = default;

    struct ActiveSample {
        const char* name;
        std::chrono::high_resolution_clock::time_point start;
        size_t depth{0};
    };

    FrameData m_currentFrame;
    FrameData m_previousFrame;
    std::vector<FrameData> m_frameHistory;
    std::vector<ActiveSample> m_activeStack;
    size_t m_maxHistory{120};

    using Clock = std::chrono::high_resolution_clock;
};

class ScopedProfile {
public:
    ScopedProfile(const char* name) {
        Profiler::instance().beginSample(name);
    }
    ~ScopedProfile() {
        Profiler::instance().endSample();
    }
    ScopedProfile(const ScopedProfile&) = delete;
    ScopedProfile& operator=(const ScopedProfile&) = delete;
};

#define CONCAT_VAR(a, b) a##b
#define PROFILE_SCOPE(name) engine::debug::ScopedProfile CONCAT_VAR(prof_, __LINE__)(name)

} // namespace engine::debug
// v2: Frame budget warnings
// v3: Timeline visualization
