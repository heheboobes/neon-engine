#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include <string>
#include <vector>
#include <functional>
#include <variant>

namespace engine::input {

enum class InputDevice {
    Keyboard,
    Mouse,
    Joystick
};

struct KeyBinding {
    sf::Keyboard::Key key = sf::Keyboard::Unknown;
};

struct MouseBinding {
    sf::Mouse::Button button = sf::Mouse::ButtonCount;
};

struct JoystickBinding {
    unsigned int joystickId = 0;
    union {
        unsigned int buttonIndex = 0;
        struct {
            sf::Joystick::Axis axis;
            float threshold;
            bool positiveDirection;
        } axis;
    };
    bool isAxis = false;
};

using BindingTarget = std::variant<KeyBinding, MouseBinding, JoystickBinding>;

struct InputBinding {
    std::string actionName;
    BindingTarget target;
    float scale = 1.0f;
    float deadZone = 0.1f;
    bool enabled = true;
};

class BindingManager {
public:
    BindingManager() = default;

    bool addBinding(const std::string& actionName, const BindingTarget& target, float scale = 1.0f);
    bool removeBinding(const std::string& actionName, std::size_t index = 0);
    void removeAllBindings(const std::string& actionName);
    void clear();

    const std::vector<InputBinding>& getBindings(const std::string& actionName) const;
    std::vector<InputBinding> getAllBindings() const;
    std::size_t getBindingCount(const std::string& actionName) const;

    bool rebind(const std::string& actionName, std::size_t index, const BindingTarget& newTarget);
    bool setBindingScale(const std::string& actionName, std::size_t index, float scale);
    bool setBindingDeadZone(const std::string& actionName, std::size_t index, float deadZone);
    bool setBindingEnabled(const std::string& actionName, std::size_t index, bool enabled);

    bool isBound(const std::string& actionName) const;
    bool hasConflicts(const BindingTarget& target, const std::string& excludeAction = "") const;
    std::vector<std::string> findConflicts(const BindingTarget& target, const std::string& excludeAction = "") const;

    static std::string bindingToString(const BindingTarget& target);
    static BindingTarget stringToBinding(const std::string& str);

private:
    std::unordered_map<std::string, std::vector<InputBinding>> m_bindings;
};

} // namespace engine::input
