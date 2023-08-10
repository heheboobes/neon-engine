#pragma once

#include <SFML/Window/Event.hpp>
#include <string>
#include <vector>
#include <functional>

namespace engine::input {

enum class InputState {
    Released,
    Pressed,
    Held
};

struct InputChord {
    std::vector<sf::Keyboard::Key> keys;
    std::vector<sf::Mouse::Button> mouseButtons;
    bool requireExact = false;

    bool operator==(const InputChord& other) const {
        return keys == other.keys && mouseButtons == other.mouseButtons;
    }
};

class InputAction {
public:
    explicit InputAction(const std::string& name);

    const std::string& getName() const;
    void setName(const std::string& name);

    void update(bool isDown);
    void reset();

    InputState getState() const;
    bool isPressed() const;
    bool isHeld() const;
    bool isReleased() const;

    void addChord(const InputChord& chord);
    void removeChord(std::size_t index);
    void clearChords();
    const std::vector<InputChord>& getChords() const;
    std::size_t getChordCount() const;

    bool matchesChord(const InputChord& chord) const;

    using ActionCallback = std::function<void(const std::string&, InputState)>;
    void setCallback(ActionCallback callback);
    void clearCallback();

    bool getConsumeInput() const;
    void setConsumeInput(bool consume);

private:
    std::string m_name;
    InputState m_state = InputState::Released;
    InputState m_previousState = InputState::Released;
    std::vector<InputChord> m_chords;
    ActionCallback m_callback;
    bool m_consumeInput = false;
};

} // namespace engine::input
// v2: Chord support
