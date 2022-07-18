#include "InputAction.h"
#include <algorithm>

namespace engine::input {

InputAction::InputAction(const std::string& name)
    : m_name(name) {
}

const std::string& InputAction::getName() const {
    return m_name;
}

void InputAction::setName(const std::string& name) {
    m_name = name;
}

void InputAction::update(bool isDown) {
    m_previousState = m_state;

    if (isDown) {
        m_state = (m_previousState == InputState::Pressed || m_previousState == InputState::Held)
                      ? InputState::Held
                      : InputState::Pressed;
    } else {
        m_state = (m_previousState == InputState::Pressed || m_previousState == InputState::Held)
                      ? InputState::Released
                      : InputState::Released;
    }

    if (m_callback && m_state != m_previousState) {
        m_callback(m_name, m_state);
    }
}

void InputAction::reset() {
    m_state = InputState::Released;
    m_previousState = InputState::Released;
}

InputState InputAction::getState() const {
    return m_state;
}

bool InputAction::isPressed() const {
    return m_state == InputState::Pressed;
}

bool InputAction::isHeld() const {
    return m_state == InputState::Held || m_state == InputState::Pressed;
}

bool InputAction::isReleased() const {
    return m_state == InputState::Released && m_previousState != InputState::Released;
}

void InputAction::addChord(const InputChord& chord) {
    m_chords.push_back(chord);
}

void InputAction::removeChord(std::size_t index) {
    if (index < m_chords.size()) {
        m_chords.erase(m_chords.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

void InputAction::clearChords() {
    m_chords.clear();
}

const std::vector<InputChord>& InputAction::getChords() const {
    return m_chords;
}

std::size_t InputAction::getChordCount() const {
    return m_chords.size();
}

bool InputAction::matchesChord(const InputChord& chord) const {
    for (const auto& c : m_chords) {
        if (c == chord) return true;
    }
    return false;
}

void InputAction::setCallback(ActionCallback callback) {
    m_callback = std::move(callback);
}

void InputAction::clearCallback() {
    m_callback = nullptr;
}

bool InputAction::getConsumeInput() const {
    return m_consumeInput;
}

void InputAction::setConsumeInput(bool consume) {
    m_consumeInput = consume;
}

} // namespace engine::input
