#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

namespace engine {
namespace systems {

enum class InputState {
    None,
    Pressed,
    Held,
    Released
};

enum class InputType {
    Keyboard,
    MouseButton,
    MouseWheel,
    JoystickButton,
    JoystickAxis
};

struct InputBinding {
    InputType type;
    union {
        sf::Keyboard::Key keyboardKey;
        sf::Mouse::Button mouseButton;
        struct { uint32_t joystickId; uint32_t button; } joystickButton;
        struct { uint32_t joystickId; sf::Joystick::Axis axis; float threshold; bool positive; } joystickAxis;
    };
    int32_t modifiers;
};

struct InputAction {
    std::string name;
    std::vector<InputBinding> bindings;
    InputState state;
    float value;
};

class InputSystem : public ecs::ISystem {
public:
    InputSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;

    void ProcessEvent(const sf::Event& event);

    void MapAction(const std::string& name, const InputBinding& binding);
    void MapAxis(const std::string& name, const InputBinding& negative, const InputBinding& positive);
    void RemoveAction(const std::string& name);
    void ClearAllBindings();

    InputState GetActionState(const std::string& name) const;
    float GetActionValue(const std::string& name) const;
    bool IsActionPressed(const std::string& name) const;
    bool IsActionJustPressed(const std::string& name) const;
    bool IsActionReleased(const std::string& name) const;

    sf::Vector2f GetMousePosition() const;
    sf::Vector2f GetMouseDelta() const;
    float GetScrollDelta() const;

    void SetMouseVisible(bool visible);
    void SetMouseGrabbed(bool grabbed);
    void SetActionCallback(std::function<void(const std::string&, InputState)> callback);

private:
    void UpdateActionStates(float dt);
    void UpdateModifierState();
    bool TestBinding(const InputBinding& binding) const;
    InputState& GetOrCreateActionState(const std::string& name);

    struct ActionState {
        InputState state;
        InputState previousState;
        float value;
        float holdTime;
    };

    std::unordered_map<std::string, ActionState> m_actionStates;
    std::vector<std::pair<std::string, InputBinding>> m_bindings;
    std::vector<std::pair<std::string, std::pair<InputBinding, InputBinding>>> m_axisBindings;

    bool m_keyState[sf::Keyboard::KeyCount];
    bool m_prevKeyState[sf::Keyboard::KeyCount];
    bool m_mouseButtonState[sf::Mouse::ButtonCount];
    bool m_prevMouseButtonState[sf::Mouse::ButtonCount];

    sf::Vector2f m_mousePosition;
    sf::Vector2f m_mouseDelta;
    float m_scrollDelta;

    bool m_modifierCtrl;
    bool m_modifierAlt;
    bool m_modifierShift;

    std::vector<std::function<void(const std::string&, InputState)>> m_actionCallbacks;
};

} // namespace systems
} // namespace engine
