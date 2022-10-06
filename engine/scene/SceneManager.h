#pragma once

#include "Scene.h"
#include <vector>
#include <memory>
#include <stack>
#include <string>

namespace engine::scene {

enum class SceneAction {
    None,
    Push,
    Pop,
    Switch
};

struct SceneCommand {
    SceneAction action{ SceneAction::None };
    std::string targetScene;
    std::unique_ptr<Scene> sceneInstance;
};

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    void pushScene(std::unique_ptr<Scene> scene);
    void popScene();
    void switchScene(std::unique_ptr<Scene> scene);

    Scene* getCurrentScene() const;
    Scene* getScene(const std::string& name) const;

    void update(float dt);
    void render();

    void onEvent(const sf::Event& event);

    void createScene(const std::string& name);
    void destroyScene(const std::string& name);
    void activateScene(const std::string& name);
    void deactivateScene(const std::string& name);

    void flushCommands();

    size_t sceneCount() const { return m_scenes.size(); }
    bool isEmpty() const { return m_stack.empty(); }

private:
    void executePush(std::unique_ptr<Scene> scene);
    void executePop();
    void executeSwitch(std::unique_ptr<Scene> scene);

    std::vector<std::unique_ptr<Scene>> m_scenes;
    std::stack<size_t> m_stack;
    std::vector<SceneCommand> m_commandQueue;
};

} // namespace engine::scene
