#include "StringUtils.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <numeric>

namespace engine::utils {

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter)
{
    std::vector<std::string> parts;
    std::istringstream stream(str);
    std::string token;
    while (std::getline(stream, token, delimiter))
        parts.push_back(token);
    return parts;
}

std::vector<std::string> StringUtils::split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> parts;
    size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        parts.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    parts.push_back(str.substr(start));
    return parts;
}

std::string StringUtils::trim(const std::string& str)
{
    return trimRight(trimLeft(str));
}

std::string StringUtils::trimLeft(const std::string& str)
{
    auto it = std::find_if(str.begin(), str.end(),
        [](unsigned char c) { return !std::isspace(c); });
    return { it, str.end() };
}

std::string StringUtils::trimRight(const std::string& str)
{
    auto it = std::find_if(str.rbegin(), str.rend(),
        [](unsigned char c) { return !std::isspace(c); });
    return { str.begin(), it.base() };
}

std::string StringUtils::join(const std::vector<std::string>& parts, const std::string& separator)
{
    if (parts.empty()) return {};
    return std::accumulate(std::next(parts.begin()), parts.end(),
        parts[0], [&](const std::string& a, const std::string& b) {
            return a + separator + b;
        });
}

std::string StringUtils::join(const std::vector<std::string>& parts, char separator)
{
    return join(parts, std::string(1, separator));
}

std::string StringUtils::replace(const std::string& str, const std::string& from, const std::string& to)
{
    std::string result = str;
    size_t pos = result.find(from);
    if (pos != std::string::npos)
        result.replace(pos, from.length(), to);
    return result;
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to)
{
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::string StringUtils::toLower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

std::string StringUtils::toUpper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

std::string StringUtils::format(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_list copy;
    va_copy(copy, args);
    int len = std::vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);
    if (len < 0) { va_end(args); return {}; }
    std::string result(static_cast<size_t>(len), '\0');
    std::vsnprintf(&result[0], result.size() + 1, fmt, args);
    va_end(args);
    return result;
}

size_t StringUtils::hash(const std::string& str)
{
    return hash(str.data(), str.size());
}

size_t StringUtils::hash(const char* data, size_t length)
{
    size_t h = 14695981039346656037ULL;
    for (size_t i = 0; i < length; ++i) {
        h ^= static_cast<size_t>(static_cast<unsigned char>(data[i]));
        h *= 1099511628211ULL;
    }
    return h;
}

bool StringUtils::wildcardMatch(const std::string& str, const std::string& pattern)
{
    size_t s = 0, p = 0, star = std::string::npos, match = 0;
    while (s < str.size()) {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == str[s])) {
            ++s; ++p;
        } else if (p < pattern.size() && pattern[p] == '*') {
            star = p++;
            match = s;
        } else if (star != std::string::npos) {
            p = star + 1;
            ++match;
            s = match;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
        ++p;
    return p == pattern.size();
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() &&
        str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() &&
        str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool StringUtils::isEmptyOrWhitespace(const std::string& str)
{
    return std::all_of(str.begin(), str.end(),
        [](unsigned char c) { return std::isspace(c); });
}

} // namespace engine::utils
