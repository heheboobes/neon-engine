#include "GameState.h"
#include <cassert>

namespace engine {
namespace core {

void GameStateManager::RegisterState(GameStateType type, const GameStateInfo& info,
                                     StateEnterFunc onEnter,
                                     StateExitFunc onExit,
                                     StateUpdateFunc onUpdate,
                                     StateRenderFunc onRender)
{
    StateEntry entry;
    entry.info = info;
    entry.onEnter = std::move(onEnter);
    entry.onExit = std::move(onExit);
    entry.onUpdate = std::move(onUpdate);
    entry.onRender = std::move(onRender);
    m_states[type] = std::move(entry);
}

void GameStateManager::PushState(GameStateType type)
{
    auto it = m_states.find(type);
    if (it == m_states.end()) return;

    m_previousState = m_stateStack.empty() ? GameStateType::None : m_stateStack.top();
    m_stateStack.push(type);

    if (it->second.onEnter)
        it->second.onEnter();
}

void GameStateManager::PopState()
{
    if (m_stateStack.empty()) return;

    GameStateType top = m_stateStack.top();
    auto it = m_states.find(top);
    if (it != m_states.end() && it->second.onExit)
        it->second.onExit();

    m_previousState = top;
    m_stateStack.pop();
}

void GameStateManager::ChangeState(GameStateType type)
{
    if (!m_stateStack.empty())
    {
        GameStateType current = m_stateStack.top();
        auto currentIt = m_states.find(current);
        if (currentIt != m_states.end() && currentIt->second.onExit)
            currentIt->second.onExit();
        m_previousState = current;
        m_stateStack.pop();
    }

    auto newIt = m_states.find(type);
    if (newIt != m_states.end())
    {
        m_stateStack.push(type);
        if (newIt->second.onEnter)
            newIt->second.onEnter();
    }
}

void GameStateManager::Update(float deltaTime)
{
    if (m_stateStack.empty()) return;

    std::stack<GameStateType> temp = m_stateStack;
    while (!temp.empty())
    {
        GameStateType type = temp.top();
        auto it = m_states.find(type);
        if (it != m_states.end() && it->second.onUpdate)
            it->second.onUpdate(deltaTime);

        if (!it->second.info.isTransparent)
            break;

        temp.pop();
    }
}

void GameStateManager::Render(float alpha)
{
    if (m_stateStack.empty()) return;

    std::stack<GameStateType> temp = m_stateStack;
    while (!temp.empty())
    {
        GameStateType type = temp.top();
        auto it = m_states.find(type);
        if (it != m_states.end() && it->second.onRender)
            it->second.onRender(alpha);

        if (!it->second.info.isTransparent)
            break;

        temp.pop();
    }
}

GameStateType GameStateManager::GetCurrentState() const
{
    return m_stateStack.empty() ? GameStateType::None : m_stateStack.top();
}

GameStateType GameStateManager::GetPreviousState() const
{
    return m_previousState;
}

bool GameStateManager::IsInState(GameStateType type) const
{
    if (m_stateStack.empty()) return false;

    std::stack<GameStateType> temp = m_stateStack;
    while (!temp.empty())
    {
        if (temp.top() == type)
            return true;
        temp.pop();
    }
    return false;
}

} // namespace core
} // namespace engine
