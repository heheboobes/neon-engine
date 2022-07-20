#include "InputBinding.h"
#include <algorithm>
#include <sstream>

namespace engine::input {

bool BindingManager::addBinding(const std::string& actionName, const BindingTarget& target, float scale) {
    InputBinding binding;
    binding.actionName = actionName;
    binding.target = target;
    binding.scale = scale;

    auto& bindings = m_bindings[actionName];
    bindings.push_back(std::move(binding));
    return true;
}

bool BindingManager::removeBinding(const std::string& actionName, std::size_t index) {
    auto it = m_bindings.find(actionName);
    if (it == m_bindings.end()) return false;
    if (index >= it->second.size()) return false;
    it->second.erase(it->second.begin() + static_cast<std::ptrdiff_t>(index));
    if (it->second.empty()) {
        m_bindings.erase(it);
    }
    return true;
}

void BindingManager::removeAllBindings(const std::string& actionName) {
    m_bindings.erase(actionName);
}

void BindingManager::clear() {
    m_bindings.clear();
}

const std::vector<InputBinding>& BindingManager::getBindings(const std::string& actionName) const {
    static const std::vector<InputBinding> empty;
    auto it = m_bindings.find(actionName);
    return it != m_bindings.end() ? it->second : empty;
}

std::vector<InputBinding> BindingManager::getAllBindings() const {
    std::vector<InputBinding> all;
    for (const auto& [name, bindings] : m_bindings) {
        all.insert(all.end(), bindings.begin(), bindings.end());
    }
    return all;
}

std::size_t BindingManager::getBindingCount(const std::string& actionName) const {
    auto it = m_bindings.find(actionName);
    return it != m_bindings.end() ? it->second.size() : 0;
}

bool BindingManager::rebind(const std::string& actionName, std::size_t index, const BindingTarget& newTarget) {
    auto it = m_bindings.find(actionName);
    if (it == m_bindings.end()) return false;
    if (index >= it->second.size()) return false;
    it->second[index].target = newTarget;
    return true;
}

bool BindingManager::setBindingScale(const std::string& actionName, std::size_t index, float scale) {
    auto it = m_bindings.find(actionName);
    if (it == m_bindings.end()) return false;
    if (index >= it->second.size()) return false;
    it->second[index].scale = scale;
    return true;
}

bool BindingManager::setBindingDeadZone(const std::string& actionName, std::size_t index, float deadZone) {
    auto it = m_bindings.find(actionName);
    if (it == m_bindings.end()) return false;
    if (index >= it->second.size()) return false;
    it->second[index].deadZone = std::clamp(deadZone, 0.0f, 1.0f);
    return true;
}

bool BindingManager::setBindingEnabled(const std::string& actionName, std::size_t index, bool enabled) {
    auto it = m_bindings.find(actionName);
    if (it == m_bindings.end()) return false;
    if (index >= it->second.size()) return false;
    it->second[index].enabled = enabled;
    return true;
}

bool BindingManager::isBound(const std::string& actionName) const {
    return m_bindings.find(actionName) != m_bindings.end() && !m_bindings.at(actionName).empty();
}

bool BindingManager::hasConflicts(const BindingTarget& target, const std::string& excludeAction) const {
    return !findConflicts(target, excludeAction).empty();
}

std::vector<std::string> BindingManager::findConflicts(const BindingTarget& target, const std::string& excludeAction) const {
    std::vector<std::string> conflicts;
    for (const auto& [name, bindings] : m_bindings) {
        if (name == excludeAction) continue;
        for (const auto& binding : bindings) {
            if (binding.target == target) {
                conflicts.push_back(name);
            }
        }
    }
    return conflicts;
}

std::string BindingManager::bindingToString(const BindingTarget& target) {
    if (std::holds_alternative<KeyBinding>(target)) {
        auto& kb = std::get<KeyBinding>(target);
        return "Keyboard:" + std::to_string(static_cast<int>(kb.key));
    } else if (std::holds_alternative<MouseBinding>(target)) {
        auto& mb = std::get<MouseBinding>(target);
        return "Mouse:" + std::to_string(static_cast<int>(mb.button));
    } else if (std::holds_alternative<JoystickBinding>(target)) {
        auto& jb = std::get<JoystickBinding>(target);
        std::ostringstream oss;
        oss << "Joystick:" << jb.joystickId << ":";
        if (jb.isAxis) {
            oss << "Axis:" << static_cast<int>(jb.axis.axis) << ":" << jb.axis.threshold;
        } else {
            oss << "Button:" << jb.buttonIndex;
        }
        return oss.str();
    }
    return "Unknown";
}

BindingTarget BindingManager::stringToBinding(const std::string& str) {
    auto colon1 = str.find(':');
    if (colon1 == std::string::npos) return KeyBinding{};

    std::string type = str.substr(0, colon1);
    std::string rest = str.substr(colon1 + 1);

    if (type == "Keyboard") {
        return KeyBinding{static_cast<sf::Keyboard::Key>(std::stoi(rest))};
    } else if (type == "Mouse") {
        return MouseBinding{static_cast<sf::Mouse::Button>(std::stoi(rest))};
    } else if (type == "Joystick") {
        auto colon2 = rest.find(':');
        if (colon2 == std::string::npos) return KeyBinding{};
        unsigned int jid = static_cast<unsigned int>(std::stoul(rest.substr(0, colon2)));
        std::string subtype = rest.substr(colon2 + 1);
        auto colon3 = subtype.find(':');
        if (colon3 == std::string::npos) return KeyBinding{};
        std::string kind = subtype.substr(0, colon3);
        std::string val = subtype.substr(colon3 + 1);
        JoystickBinding jb;
        jb.joystickId = jid;
        if (kind == "Button") {
            jb.isAxis = false;
            jb.buttonIndex = static_cast<unsigned int>(std::stoul(val));
        } else if (kind == "Axis") {
            jb.isAxis = true;
            auto colon4 = val.find(':');
            if (colon4 != std::string::npos) {
                jb.axis.axis = static_cast<sf::Joystick::Axis>(std::stoi(val.substr(0, colon4)));
                jb.axis.threshold = std::stof(val.substr(colon4 + 1));
            }
        }
        return jb;
    }
    return KeyBinding{};
}

} // namespace engine::input
