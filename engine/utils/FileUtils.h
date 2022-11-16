#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <filesystem>

namespace engine::utils {

namespace fs = std::filesystem;

struct FileWatchEntry {
    fs::path path;
    fs::file_time_type lastWriteTime;
};

class FileUtils {
public:
    static std::string readAllText(const fs::path& path);
    static bool writeAllText(const fs::path& path, const std::string& content);

    static std::vector<fs::path> listDirectory(const fs::path& dir,
        bool recursive = false, const std::string& extension = "");

    static std::string sanitizePath(const std::string& path);
    static bool hasExtension(const fs::path& path, const std::string& ext);

    static bool exists(const fs::path& path);
    static uintmax_t fileSize(const fs::path& path);

    FileUtils() = delete;

    static void watchFile(const fs::path& path,
        std::function<void(const fs::path&)> callback);
    static void pollWatchers();

private:
    static std::vector<FileWatchEntry>& watchers();
};

} // namespace engine::utils
