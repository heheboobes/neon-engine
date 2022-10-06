#include "SceneManager.h"
#include <algorithm>
#include <cassert>

namespace engine::scene {

SceneManager::SceneManager()
{
    m_scenes.reserve(16);
}

SceneManager::~SceneManager() = default;

void SceneManager::pushScene(std::unique_ptr<Scene> scene)
{
    SceneCommand cmd;
    cmd.action = SceneAction::Push;
    cmd.sceneInstance = std::move(scene);
    m_commandQueue.push_back(std::move(cmd));
}

void SceneManager::popScene()
{
    SceneCommand cmd;
    cmd.action = SceneAction::Pop;
    m_commandQueue.push_back(std::move(cmd));
}

void SceneManager::switchScene(std::unique_ptr<Scene> scene)
{
    SceneCommand cmd;
    cmd.action = SceneAction::Switch;
    cmd.sceneInstance = std::move(scene);
    m_commandQueue.push_back(std::move(cmd));
}

Scene* SceneManager::getCurrentScene() const
{
    if (m_stack.empty())
        return nullptr;
    size_t idx = m_stack.top();
    if (idx >= m_scenes.size())
        return nullptr;
    return m_scenes[idx].get();
}

Scene* SceneManager::getScene(const std::string& name) const
{
    for (const auto& s : m_scenes) {
        if (s && s->getName() == name)
            return s.get();
    }
    return nullptr;
}

void SceneManager::update(float dt)
{
    flushCommands();
    if (auto* cur = getCurrentScene())
        cur->update(dt);
}

void SceneManager::render()
{
    if (auto* cur = getCurrentScene())
        cur->render();
}

void SceneManager::onEvent(const sf::Event& event)
{
    (void)event;
}

void SceneManager::createScene(const std::string& name)
{
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&](const auto& s) { return s && s->getName() == name; });
    if (it != m_scenes.end())
        return;
    m_scenes.push_back(std::make_unique<Scene>(name));
}

void SceneManager::destroyScene(const std::string& name)
{
    for (size_t i = 0; i < m_scenes.size(); ++i) {
        if (m_scenes[i] && m_scenes[i]->getName() == name) {
            m_scenes[i].reset();
            break;
        }
    }
}

void SceneManager::activateScene(const std::string& name)
{
    for (size_t i = 0; i < m_scenes.size(); ++i) {
        if (m_scenes[i] && m_scenes[i]->getName() == name) {
            m_stack.push(i);
            return;
        }
    }
}

void SceneManager::deactivateScene(const std::string& name)
{
    std::stack<size_t> filtered;
    while (!m_stack.empty()) {
        size_t idx = m_stack.top();
        m_stack.pop();
        if (m_scenes[idx] && m_scenes[idx]->getName() != name)
            filtered.push(idx);
    }
    m_stack = std::move(filtered);
}

void SceneManager::flushCommands()
{
    for (auto& cmd : m_commandQueue) {
        switch (cmd.action) {
        case SceneAction::Push:
            executePush(std::move(cmd.sceneInstance));
            break;
        case SceneAction::Pop:
            executePop();
            break;
        case SceneAction::Switch:
            executeSwitch(std::move(cmd.sceneInstance));
            break;
        default:
            break;
        }
    }
    m_commandQueue.clear();
}

void SceneManager::executePush(std::unique_ptr<Scene> scene)
{
    if (!scene)
        return;
    m_scenes.push_back(std::move(scene));
    m_stack.push(m_scenes.size() - 1);
}

void SceneManager::executePop()
{
    if (m_stack.empty())
        return;
    size_t idx = m_stack.top();
    m_stack.pop();
    if (idx < m_scenes.size())
        m_scenes[idx].reset();
}

void SceneManager::executeSwitch(std::unique_ptr<Scene> scene)
{
    if (!scene)
        return;
    while (!m_stack.empty()) {
        size_t idx = m_stack.top();
        m_stack.pop();
        if (idx < m_scenes.size())
            m_scenes[idx].reset();
    }
    m_scenes.push_back(std::move(scene));
    m_stack.push(m_scenes.size() - 1);
}

} // namespace engine::scene
