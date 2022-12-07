#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <functional>
#include <chrono>
#include <unordered_set>

namespace engine::debug {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info  = 2,
    Warn  = 3,
    Error = 4,
    Fatal = 5
};

const char* logLevelToString(LogLevel level);
const char* logLevelToAnsiColor(LogLevel level);

struct LogMessage {
    LogLevel level{LogLevel::Info};
    std::string category;
    std::string message;
    std::string file;
    int line{0};
    std::chrono::system_clock::time_point timestamp;
};

using LogCallback = std::function<void(const LogMessage&)>;

class Logger {
public:
    static Logger& instance();

    void setMinLevel(LogLevel level);
    LogLevel getMinLevel() const;

    void setCategoryFilter(const std::unordered_set<std::string>& categories);
    void clearCategoryFilter();
    bool isCategoryAllowed(const std::string& category) const;

    void addConsoleSink(bool colored = true);
    void addFileSink(const std::string& filepath, bool append = false);
    void addCallbackSink(LogCallback callback);

    void removeAllSinks();

    void log(LogLevel level, const std::string& category,
             const std::string& message, const char* file = nullptr, int line = 0);

    void flush();

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    struct Sink {
        enum Type { Console, File, Callback };
        Type type;
        bool colored{true};
        std::ofstream fileStream;
        LogCallback callback;
    };

    LogLevel m_minLevel{LogLevel::Trace};
    std::vector<Sink> m_sinks;
    std::unordered_set<std::string> m_categoryFilter;
    bool m_filterEnabled{false};
    std::mutex m_mutex;

    std::string formatMessage(const LogMessage& msg, bool ansi) const;
    void writeToSink(const Sink& sink, const LogMessage& msg);
};

#define LOG_TRACE(cat, msg)    engine::debug::Logger::instance().log(engine::debug::LogLevel::Trace, cat, msg, __FILE__, __LINE__)
#define LOG_DEBUG(cat, msg)    engine::debug::Logger::instance().log(engine::debug::LogLevel::Debug, cat, msg, __FILE__, __LINE__)
#define LOG_INFO(cat, msg)     engine::debug::Logger::instance().log(engine::debug::LogLevel::Info, cat, msg, __FILE__, __LINE__)
#define LOG_WARN(cat, msg)     engine::debug::Logger::instance().log(engine::debug::LogLevel::Warn, cat, msg, __FILE__, __LINE__)
#define LOG_ERROR(cat, msg)    engine::debug::Logger::instance().log(engine::debug::LogLevel::Error, cat, msg, __FILE__, __LINE__)
#define LOG_FATAL(cat, msg)    engine::debug::Logger::instance().log(engine::debug::LogLevel::Fatal, cat, msg, __FILE__, __LINE__)

} // namespace engine::debug
