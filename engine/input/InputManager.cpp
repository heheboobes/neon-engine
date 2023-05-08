#include "InputManager.h"
#include <algorithm>
#include <cmath>

namespace engine::input {

InputManager::InputManager() {
    m_keyStates.fill(false);
    m_prevKeyStates.fill(false);
    m_keyPressed.fill(false);
    m_keyReleased.fill(false);
}

bool InputManager::init() {
    m_initialized = true;
    return true;
}

void InputManager::shutdown() {
    m_actions.clear();
    m_axisBindings.clear();
    m_keyStates.fill(false);
    m_prevKeyStates.fill(false);
    m_initialized = false;
}

void InputManager::processEvent(const sf::Event& event) {
    if (m_eventCallback) {
        m_eventCallback(event);
    }

    processKeyboardEvent(event);
    processMouseEvent(event);
    processJoystickEvent(event);
}

void InputManager::update(float dt) {
    (void)dt;
    updateKeyboardState();
    updateMouseState();
    updateJoystickState();
    evaluateActionBindings();
}

bool InputManager::registerAction(const std::string& name) {
    if (m_actions.find(name) != m_actions.end()) return false;
    m_actions.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(name));
    return true;
}

bool InputManager::unregisterAction(const std::string& name) {
    return m_actions.erase(name) > 0;
}

bool InputManager::hasAction(const std::string& name) const {
    return m_actions.find(name) != m_actions.end();
}

InputAction* InputManager::getAction(const std::string& name) {
    auto it = m_actions.find(name);
    return it != m_actions.end() ? &it->second : nullptr;
}

bool InputManager::isKeyDown(sf::Keyboard::Key key) const {
    return key < sf::Keyboard::KeyCount && m_keyStates[key];
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const {
    return key < sf::Keyboard::KeyCount && m_keyPressed[key];
}

bool InputManager::isKeyReleased(sf::Keyboard::Key key) const {
    return key < sf::Keyboard::KeyCount && m_keyReleased[key];
}

bool InputManager::isMouseButtonDown(sf::Mouse::Button button) const {
    auto idx = static_cast<std::size_t>(button);
    return idx < m_mouse.buttons.size() && m_mouse.buttons[idx];
}

bool InputManager::isMouseButtonPressed(sf::Mouse::Button button) const {
    static std::array<bool, 5> prev = {};
    auto idx = static_cast<std::size_t>(button);
    if (idx >= m_mouse.buttons.size()) return false;
    return m_mouse.buttons[idx] && !prev[idx];
}

float InputManager::getMouseX() const {
    return m_mouse.x;
}

float InputManager::getMouseY() const {
    return m_mouse.y;
}

float InputManager::getMouseDeltaX() const {
    return m_mouse.dx;
}

float InputManager::getMouseDeltaY() const {
    return m_mouse.dy;
}

float InputManager::getScrollDelta() const {
    return m_mouse.scrollDelta;
}

bool InputManager::isJoystickConnected(unsigned int id) const {
    return sf::Joystick::isConnected(id);
}

float InputManager::getJoystickAxisValue(unsigned int id, sf::Joystick::Axis axis) const {
    return sf::Joystick::getAxisPosition(id, axis);
}

bool InputManager::isJoystickButtonDown(unsigned int id, unsigned int button) const {
    return sf::Joystick::isButtonPressed(id, button);
}

void InputManager::addAxisBinding(const std::string& axisName, const AxisBinding& binding) {
    m_axisBindings[axisName].push_back(binding);
}

void InputManager::removeAxisBinding(const std::string& axisName, std::size_t index) {
    auto it = m_axisBindings.find(axisName);
    if (it == m_axisBindings.end()) return;
    if (index < it->second.size()) {
        it->second.erase(it->second.begin() + static_cast<std::ptrdiff_t>(index));
        if (it->second.empty()) {
            m_axisBindings.erase(it);
        }
    }
}

float InputManager::getAxisValue(const std::string& axisName) const {
    auto it = m_axisBindings.find(axisName);
    if (it == m_axisBindings.end()) return 0.0f;

    float value = 0.0f;
    for (const auto& binding : it->second) {
        float v = 0.0f;
        if (binding.negativeKey != sf::Keyboard::Unknown && m_keyStates[binding.negativeKey]) v -= 1.0f;
        if (binding.positiveKey != sf::Keyboard::Unknown && m_keyStates[binding.positiveKey]) v += 1.0f;

        if (sf::Joystick::isConnected(binding.joystickId)) {
            float joy = sf::Joystick::getAxisPosition(binding.joystickId, binding.joystickAxis) / 100.0f;
            if (std::abs(joy) > binding.deadZone) {
                v += joy;
            }
        }

        v *= binding.sensitivity;
        if (binding.invert) v = -v;
        value += v;
    }
    return std::clamp(value, -1.0f, 1.0f);
}

bool InputManager::hasAxis(const std::string& axisName) const {
    return m_axisBindings.find(axisName) != m_axisBindings.end();
}

void InputManager::setMouseVisible(bool visible) {
    (void)visible;
}

void InputManager::setMouseCursor(const sf::Cursor& cursor) {
    (void)cursor;
}

void InputManager::setMousePosition(int x, int y, const sf::Window& window) {
    sf::Mouse::setPosition(sf::Vector2i(x, y), window);
}

void InputManager::mapEventToAction(const std::string& actionName, const BindingTarget& target, float scale) {
    if (!hasAction(actionName)) {
        registerAction(actionName);
    }
    m_bindingManager.addBinding(actionName, target, scale);
}

void InputManager::rebindAction(const std::string& actionName, std::size_t bindingIndex, const BindingTarget& newTarget) {
    m_bindingManager.rebind(actionName, bindingIndex, newTarget);
}

void InputManager::setEventCallback(EventCallback callback) {
    m_eventCallback = std::move(callback);
}

const BindingManager& InputManager::getBindingManager() const {
    return m_bindingManager;
}

BindingManager& InputManager::getBindingManager() {
    return m_bindingManager;
}

void InputManager::updateKeyboardState() {
    for (int i = 0; i < sf::Keyboard::KeyCount; ++i) {
        auto key = static_cast<sf::Keyboard::Key>(i);
        m_keyPressed[i] = m_keyStates[i] && !m_prevKeyStates[i];
        m_keyReleased[i] = !m_keyStates[i] && m_prevKeyStates[i];
        m_prevKeyStates[i] = m_keyStates[i];
    }
}

void InputManager::updateMouseState() {
    m_mouse.dx = 0.0f;
    m_mouse.dy = 0.0f;
    m_mouse.scrollDelta = 0.0f;
}

void InputManager::updateJoystickState() {
    for (unsigned int i = 0; i < sf::Joystick::Count; ++i) {
        if (sf::Joystick::isConnected(i)) {
            sf::Joystick::update();
        }
    }
}

void InputManager::evaluateActionBindings() {
    for (auto& [name, action] : m_actions) {
        bool active = false;
        auto bindings = m_bindingManager.getBindings(name);
        for (const auto& binding : bindings) {
            if (!binding.enabled) continue;
            if (std::holds_alternative<KeyBinding>(binding.target)) {
                auto& kb = std::get<KeyBinding>(binding.target);
                if (m_keyStates[kb.key]) {
                    active = true;
                    break;
                }
            } else if (std::holds_alternative<MouseBinding>(binding.target)) {
                auto& mb = std::get<MouseBinding>(binding.target);
                auto idx = static_cast<std::size_t>(mb.button);
                if (idx < m_mouse.buttons.size() && m_mouse.buttons[idx]) {
                    active = true;
                    break;
                }
            } else if (std::holds_alternative<JoystickBinding>(binding.target)) {
                auto& jb = std::get<JoystickBinding>(binding.target);
                if (sf::Joystick::isConnected(jb.joystickId)) {
                    if (jb.isAxis) {
                        float val = sf::Joystick::getAxisPosition(jb.joystickId, jb.axis.axis) / 100.0f;
                        if (jb.axis.positiveDirection && val > jb.axis.threshold) {
                            active = true;
                            break;
                        }
                        if (!jb.axis.positiveDirection && val < -jb.axis.threshold) {
                            active = true;
                            break;
                        }
                    } else {
                        if (sf::Joystick::isButtonPressed(jb.joystickId, jb.buttonIndex)) {
                            active = true;
                            break;
                        }
                    }
                }
            }
        }
        action.update(active);
    }
}

void InputManager::processKeyboardEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code < sf::Keyboard::KeyCount) {
        m_keyStates[event.key.code] = true;
    } else if (event.type == sf::Event::KeyReleased && event.key.code < sf::Keyboard::KeyCount) {
        m_keyStates[event.key.code] = false;
    }
}

void InputManager::processMouseEvent(const sf::Event& event) {
    switch (event.type) {
    case sf::Event::MouseMoved:
        m_mouse.dx = static_cast<float>(event.mouseMove.x) - m_mouse.x;
        m_mouse.dy = static_cast<float>(event.mouseMove.y) - m_mouse.y;
        m_mouse.x = static_cast<float>(event.mouseMove.x);
        m_mouse.y = static_cast<float>(event.mouseMove.y);
        break;
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button < sf::Mouse::ButtonCount) {
            m_mouse.buttons[event.mouseButton.button] = true;
        }
        break;
    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button < sf::Mouse::ButtonCount) {
            m_mouse.buttons[event.mouseButton.button] = false;
        }
        break;
    case sf::Event::MouseWheelScrolled:
        m_mouse.scrollDelta = event.mouseWheelScroll.delta;
        break;
    default:
        break;
    }
}

void InputManager::processJoystickEvent(const sf::Event& event) {
    (void)event;
}

} // namespace engine::input
// v2: Key repeat handling
