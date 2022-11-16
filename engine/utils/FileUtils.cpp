#include "FileUtils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace engine::utils {

std::string FileUtils::readAllText(const fs::path& path)
{
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in.is_open())
        return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool FileUtils::writeAllText(const fs::path& path, const std::string& content)
{
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!out.is_open())
        return false;
    out.write(content.data(), static_cast<std::streamsize>(content.size()));
    return out.good();
}

std::vector<fs::path> FileUtils::listDirectory(const fs::path& dir,
    bool recursive, const std::string& extension)
{
    std::vector<fs::path> result;
    if (!fs::exists(dir) || !fs::is_directory(dir))
        return result;

    auto iter = recursive
        ? fs::recursive_directory_iterator(dir)
        : fs::directory_iterator(dir);

    for (const auto& entry : iter) {
        if (entry.is_regular_file()) {
            if (extension.empty() || entry.path().extension() == extension)
                result.push_back(entry.path());
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::string FileUtils::sanitizePath(const std::string& path)
{
    std::string result;
    result.reserve(path.size());
    for (char c : path) {
        if (c == '/' || c == '\\')
            result += fs::path::preferred_separator;
        else if (std::isprint(static_cast<unsigned char>(c)) && c != '<' && c != '>' && c != '"' && c != '|')
            result += c;
    }
    return result;
}

bool FileUtils::hasExtension(const fs::path& path, const std::string& ext)
{
    auto fileExt = path.extension().string();
    std::string lhs, rhs;
    lhs.reserve(fileExt.size());
    rhs.reserve(ext.size());
    for (auto c : fileExt) lhs.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    for (auto c : ext) rhs.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    return lhs == rhs;
}

bool FileUtils::exists(const fs::path& path)
{
    return fs::exists(path);
}

uintmax_t FileUtils::fileSize(const fs::path& path)
{
    if (!fs::exists(path))
        return 0;
    return fs::file_size(path);
}

void FileUtils::watchFile(const fs::path& path,
    std::function<void(const fs::path&)> callback)
{
    (void)callback;
    FileWatchEntry entry;
    entry.path = path;
    entry.lastWriteTime = fs::exists(path)
        ? fs::last_write_time(path)
        : fs::file_time_type::min();
    watchers().push_back(entry);
}

void FileUtils::pollWatchers()
{
    for (auto& w : watchers()) {
        if (!fs::exists(w.path))
            continue;
        auto current = fs::last_write_time(w.path);
        if (current != w.lastWriteTime) {
            w.lastWriteTime = current;
        }
    }
}

std::vector<FileWatchEntry>& FileUtils::watchers()
{
    static std::vector<FileWatchEntry> s_watchers;
    return s_watchers;
}

} // namespace engine::utils
