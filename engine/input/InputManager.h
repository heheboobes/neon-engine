#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include "InputAction.h"
#include "InputBinding.h"
#include <string>
#include <unordered_map>
#include <array>
#include <functional>

namespace engine::input {

struct AxisBinding {
    sf::Keyboard::Key negativeKey = sf::Keyboard::Unknown;
    sf::Keyboard::Key positiveKey = sf::Keyboard::Unknown;
    sf::Joystick::Axis joystickAxis = sf::Joystick::X;
    unsigned int joystickId = 0;
    float sensitivity = 1.0f;
    float deadZone = 0.1f;
    bool invert = false;
};

struct MouseState {
    float x = 0.0f;
    float y = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float scrollDelta = 0.0f;
    std::array<bool, 5> buttons = {};
};

class InputManager {
public:
    InputManager();
    ~InputManager() = default;

    bool init();
    void shutdown();

    void processEvent(const sf::Event& event);
    void update(float dt);

    bool registerAction(const std::string& name);
    bool unregisterAction(const std::string& name);
    bool hasAction(const std::string& name) const;
    InputAction* getAction(const std::string& name);

    bool isKeyDown(sf::Keyboard::Key key) const;
    bool isKeyPressed(sf::Keyboard::Key key) const;
    bool isKeyReleased(sf::Keyboard::Key key) const;

    bool isMouseButtonDown(sf::Mouse::Button button) const;
    bool isMouseButtonPressed(sf::Mouse::Button button) const;
    float getMouseX() const;
    float getMouseY() const;
    float getMouseDeltaX() const;
    float getMouseDeltaY() const;
    float getScrollDelta() const;

    bool isJoystickConnected(unsigned int id) const;
    float getJoystickAxisValue(unsigned int id, sf::Joystick::Axis axis) const;
    bool isJoystickButtonDown(unsigned int id, unsigned int button) const;

    void addAxisBinding(const std::string& axisName, const AxisBinding& binding);
    void removeAxisBinding(const std::string& axisName, std::size_t index = 0);
    float getAxisValue(const std::string& axisName) const;
    bool hasAxis(const std::string& axisName) const;

    void setMouseVisible(bool visible);
    void setMouseCursor(const sf::Cursor& cursor);
    void setMousePosition(int x, int y, const sf::Window& window);

    void mapEventToAction(const std::string& actionName, const BindingTarget& target, float scale = 1.0f);
    void rebindAction(const std::string& actionName, std::size_t bindingIndex, const BindingTarget& newTarget);

    using EventCallback = std::function<void(const sf::Event&)>;
    void setEventCallback(EventCallback callback);

    const BindingManager& getBindingManager() const;
    BindingManager& getBindingManager();

private:
    void updateKeyboardState();
    void updateMouseState();
    void updateJoystickState();
    void evaluateActionBindings();
    void processKeyboardEvent(const sf::Event& event);
    void processMouseEvent(const sf::Event& event);
    void processJoystickEvent(const sf::Event& event);

    std::unordered_map<std::string, InputAction> m_actions;
    BindingManager m_bindingManager;
    std::unordered_map<std::string, std::vector<AxisBinding>> m_axisBindings;

    std::array<bool, sf::Keyboard::KeyCount> m_keyStates = {};
    std::array<bool, sf::Keyboard::KeyCount> m_prevKeyStates = {};
    std::array<bool, sf::Keyboard::KeyCount> m_keyPressed = {};
    std::array<bool, sf::Keyboard::KeyCount> m_keyReleased = {};
    MouseState m_mouse;
    EventCallback m_eventCallback;
    bool m_initialized = false;
};

} // namespace engine::input
