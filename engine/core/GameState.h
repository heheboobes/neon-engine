#ifndef ENGINE_CORE_GAME_STATE_H
#define ENGINE_CORE_GAME_STATE_H

#include <cstdint>
#include <string>
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>

namespace engine {
namespace core {

enum class GameStateType : uint8_t {
    None = 0,
    Boot,
    Loading,
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Victory,
    Settings,
    Credits,
    Custom = 100
};

struct GameStateInfo {
    GameStateType type = GameStateType::None;
    std::string name = "Unknown";
    bool isTransparent = false;
    bool isOverlay = false;
    bool blocksUpdates = true;
};

class GameStateManager {
public:
    using StateEnterFunc = std::function<void()>;
    using StateExitFunc = std::function<void()>;
    using StateUpdateFunc = std::function<void(float)>;
    using StateRenderFunc = std::function<void(float)>;

    GameStateManager() = default;
    ~GameStateManager() = default;

    void RegisterState(GameStateType type, const GameStateInfo& info,
                       StateEnterFunc onEnter,
                       StateExitFunc onExit,
                       StateUpdateFunc onUpdate,
                       StateRenderFunc onRender);

    void PushState(GameStateType type);
    void PopState();
    void ChangeState(GameStateType type);

    void Update(float deltaTime);
    void Render(float alpha);

    GameStateType GetCurrentState() const;
    GameStateType GetPreviousState() const;
    size_t GetStateStackDepth() const { return m_stateStack.size(); }

    bool IsInState(GameStateType type) const;
    bool IsTransitioning() const { return m_transitionPending; }

private:
    struct StateEntry {
        GameStateInfo info;
        StateEnterFunc onEnter;
        StateExitFunc onExit;
        StateUpdateFunc onUpdate;
        StateRenderFunc onRender;
    };

    std::stack<GameStateType> m_stateStack;
    GameStateType m_previousState = GameStateType::None;
    std::unordered_map<GameStateType, StateEntry> m_states;
    bool m_transitionPending = false;
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_GAME_STATE_H
