#pragma once

#include <string>
#include <vector>
#include <functional>

namespace engine::utils {

class StringUtils {
public:
    StringUtils() = delete;

    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);

    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);

    static std::string join(const std::vector<std::string>& parts, const std::string& separator);
    static std::string join(const std::vector<std::string>& parts, char separator);

    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);

    static std::string format(const char* fmt, ...);

    static size_t hash(const std::string& str);
    static size_t hash(const char* data, size_t length);

    static bool wildcardMatch(const std::string& str, const std::string& pattern);

    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);

    static bool isEmptyOrWhitespace(const std::string& str);
};

} // namespace engine::utils
