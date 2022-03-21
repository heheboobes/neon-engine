#include "InputSystem.h"
#include <algorithm>
#include <cmath>

namespace engine {
namespace systems {

InputSystem::InputSystem()
    : m_scrollDelta(0.0f)
    , m_modifierCtrl(false)
    , m_modifierAlt(false)
    , m_modifierShift(false)
{
    SetName("InputSystem");
    SetPriority(0);

    for (int i = 0; i < sf::Keyboard::KeyCount; ++i)
    {
        m_keyState[i] = false;
        m_prevKeyState[i] = false;
    }

    for (int i = 0; i < sf::Mouse::ButtonCount; ++i)
    {
        m_mouseButtonState[i] = false;
        m_prevMouseButtonState[i] = false;
    }
}

void InputSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    (void)world;
    UpdateModifierState();
    UpdateActionStates(deltaTime);

    std::copy(std::begin(m_keyState), std::end(m_keyState), std::begin(m_prevKeyState));
    std::copy(std::begin(m_mouseButtonState), std::end(m_mouseButtonState), std::begin(m_prevMouseButtonState));
}

void InputSystem::ProcessEvent(const sf::Event& event)
{
    switch (event.type)
    {
    case sf::Event::KeyPressed:
        if (event.key.code < sf::Keyboard::KeyCount)
            m_keyState[event.key.code] = true;
        break;

    case sf::Event::KeyReleased:
        if (event.key.code < sf::Keyboard::KeyCount)
            m_keyState[event.key.code] = false;
        break;

    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button < sf::Mouse::ButtonCount)
            m_mouseButtonState[event.mouseButton.button] = true;
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button < sf::Mouse::ButtonCount)
            m_mouseButtonState[event.mouseButton.button] = false;
        break;

    case sf::Event::MouseMoved:
        m_mouseDelta = sf::Vector2f(
            static_cast<float>(event.mouseMove.x) - m_mousePosition.x,
            static_cast<float>(event.mouseMove.y) - m_mousePosition.y
        );
        m_mousePosition = sf::Vector2f(
            static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y)
        );
        break;

    case sf::Event::MouseWheelScrolled:
        m_scrollDelta = event.mouseWheelScroll.delta;
        break;

    default:
        break;
    }
}

void InputSystem::MapAction(const std::string& name, const InputBinding& binding)
{
    m_bindings.emplace_back(name, binding);

    auto& state = GetOrCreateActionState(name);
    (void)state;
}

void InputSystem::MapAxis(const std::string& name, const InputBinding& negative, const InputBinding& positive)
{
    m_axisBindings.emplace_back(name, std::make_pair(negative, positive));

    auto& state = GetOrCreateActionState(name);
    (void)state;
}

void InputSystem::RemoveAction(const std::string& name)
{
    m_actionStates.erase(name);

    m_bindings.erase(
        std::remove_if(m_bindings.begin(), m_bindings.end(),
            [&name](const auto& pair) { return pair.first == name; }),
        m_bindings.end()
    );

    m_axisBindings.erase(
        std::remove_if(m_axisBindings.begin(), m_axisBindings.end(),
            [&name](const auto& pair) { return pair.first == name; }),
        m_axisBindings.end()
    );
}

void InputSystem::ClearAllBindings()
{
    m_bindings.clear();
    m_axisBindings.clear();
    m_actionStates.clear();
}

InputState InputSystem::GetActionState(const std::string& name) const
{
    auto it = m_actionStates.find(name);
    if (it != m_actionStates.end())
        return it->second.state;
    return InputState::None;
}

float InputSystem::GetActionValue(const std::string& name) const
{
    auto it = m_actionStates.find(name);
    if (it != m_actionStates.end())
        return it->second.value;
    return 0.0f;
}

bool InputSystem::IsActionPressed(const std::string& name) const
{
    auto state = GetActionState(name);
    return state == InputState::Pressed || state == InputState::Held;
}

bool InputSystem::IsActionJustPressed(const std::string& name) const
{
    return GetActionState(name) == InputState::Pressed;
}

bool InputSystem::IsActionReleased(const std::string& name) const
{
    return GetActionState(name) == InputState::Released;
}

sf::Vector2f InputSystem::GetMousePosition() const
{
    return m_mousePosition;
}

sf::Vector2f InputSystem::GetMouseDelta() const
{
    return m_mouseDelta;
}

float InputSystem::GetScrollDelta() const
{
    return m_scrollDelta;
}

void InputSystem::SetMouseVisible(bool visible)
{
    (void)visible;
}

void InputSystem::SetMouseGrabbed(bool grabbed)
{
    (void)grabbed;
}

void InputSystem::SetActionCallback(std::function<void(const std::string&, InputState)> callback)
{
    m_actionCallbacks.push_back(std::move(callback));
}

void InputSystem::UpdateActionStates(float dt)
{
    for (auto& action : m_actionStates)
    {
        action.second.previousState = action.second.state;
        action.second.state = InputState::None;
        action.second.value = 0.0f;
    }

    for (const auto& [name, binding] : m_bindings)
    {
        auto& state = GetOrCreateActionState(name);
        bool pressed = TestBinding(binding);

        if (pressed)
        {
            if (state.previousState == InputState::None || state.previousState == InputState::Released)
            {
                state.state = InputState::Pressed;
            }
            else
            {
                state.state = InputState::Held;
            }
            state.value = 1.0f;
            state.holdTime += dt;
        }
        else
        {
            if (state.previousState == InputState::Pressed || state.previousState == InputState::Held)
            {
                state.state = InputState::Released;
            }
            else
            {
                state.state = InputState::None;
            }
            state.value = 0.0f;
            state.holdTime = 0.0f;
        }
    }

    for (const auto& [name, bindings] : m_axisBindings)
    {
        auto& state = GetOrCreateActionState(name);
        float axisValue = 0.0f;

        if (TestBinding(bindings.first))
            axisValue -= 1.0f;
        if (TestBinding(bindings.second))
            axisValue += 1.0f;

        state.value = axisValue;

        if (std::abs(axisValue) > 0.01f)
        {
            if (state.previousState == InputState::None || state.previousState == InputState::Released)
                state.state = InputState::Pressed;
            else
                state.state = InputState::Held;
        }
        else
        {
            if (state.previousState == InputState::Pressed || state.previousState == InputState::Held)
                state.state = InputState::Released;
            else
                state.state = InputState::None;
        }
    }

    for (const auto& [name, state] : m_actionStates)
    {
        if (state.state != state.previousState)
        {
            for (auto& callback : m_actionCallbacks)
            {
                callback(name, state.state);
            }
        }
    }
}

void InputSystem::UpdateModifierState()
{
    m_modifierCtrl = m_keyState[sf::Keyboard::LControl] || m_keyState[sf::Keyboard::RControl];
    m_modifierAlt = m_keyState[sf::Keyboard::LAlt] || m_keyState[sf::Keyboard::RAlt];
    m_modifierShift = m_keyState[sf::Keyboard::LShift] || m_keyState[sf::Keyboard::RShift];
}

bool InputSystem::TestBinding(const InputBinding& binding) const
{
    switch (binding.type)
    {
    case InputType::Keyboard:
        if (binding.keyboardKey < sf::Keyboard::KeyCount)
            return m_keyState[binding.keyboardKey];
        return false;

    case InputType::MouseButton:
        if (binding.mouseButton < sf::Mouse::ButtonCount)
            return m_mouseButtonState[binding.mouseButton];
        return false;

    default:
        return false;
    }
}

InputState& InputSystem::GetOrCreateActionState(const std::string& name)
{
    auto it = m_actionStates.find(name);
    if (it == m_actionStates.end())
    {
        ActionState state;
        state.state = InputState::None;
        state.previousState = InputState::None;
        state.value = 0.0f;
        state.holdTime = 0.0f;
        auto result = m_actionStates.emplace(name, state);
        return result.first->second;
    }
    return it->second;
}

} // namespace systems
} // namespace engine

