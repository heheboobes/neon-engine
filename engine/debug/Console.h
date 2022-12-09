#pragma once

#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <unordered_map>
#include <cstdint>

namespace engine::debug {

using ConsoleCommand = std::function<void(const std::vector<std::string>&)>;

struct ConsoleCommandInfo {
    std::string name;
    std::string description;
    std::string usage;
    ConsoleCommand callback;
};

struct ConsoleVariable {
    std::string name;
    std::string value;
    std::string description;
    bool readOnly{false};
};

struct ConsoleEntry {
    enum Type { Input, Output, Info, Warning, Error };
    Type type{Output};
    std::string text;
    std::string timestamp;
};

class Console {
public:
    static Console& instance();

    void toggle();
    bool isOpen() const;
    void setOpen(bool open);

    void registerCommand(const std::string& name, const std::string& description,
                         const std::string& usage, ConsoleCommand callback);
    void unregisterCommand(const std::string& name);
    bool executeCommand(const std::string& input);

    void registerVariable(const std::string& name, const std::string& value,
                          const std::string& description, bool readOnly = false);
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;

    void print(const std::string& text);
    void printInfo(const std::string& text);
    void printWarning(const std::string& text);
    void printError(const std::string& text);

    void setMaxHistory(size_t count);
    const std::deque<ConsoleEntry>& getEntries() const;

    std::vector<std::string> autocomplete(const std::string& prefix) const;
    std::string getPreviousHistory();
    std::string getNextHistory();

    void update(float dt);
    void render();

private:
    Console() = default;

    bool m_open{false};
    bool m_openNext{false};
    float m_animation{0.0f};
    std::string m_input;
    std::deque<ConsoleEntry> m_entries;
    std::vector<std::string> m_commandHistory;
    size_t m_historyIndex{0};
    size_t m_maxEntries{500};

    std::unordered_map<std::string, ConsoleCommandInfo> m_commands;
    std::unordered_map<std::string, ConsoleVariable> m_variables;
};

} // namespace engine::debug
