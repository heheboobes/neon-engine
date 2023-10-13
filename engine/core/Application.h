#ifndef ENGINE_CORE_APPLICATION_H
#define ENGINE_CORE_APPLICATION_H

#include "EngineConfig.h"
#include "GameLoop.h"
#include "Layer.h"
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include <string>

namespace engine {
namespace core {

enum class AppStatus {
    Uninitialized,
    Initializing,
    Running,
    Paused,
    Stopping,
    Exited
};

class Application {
public:
    Application();
    virtual ~Application();

    int Run();
    void Shutdown();

    virtual bool Init();
    virtual void OnUpdate(float deltaTime);
    virtual void OnFixedUpdate(float fixedDt);
    virtual void OnRender(float alpha);
    virtual void OnEvent(sf::Event& event);

    void SetConfig(const EngineConfig& config) { m_config = config; }
    const EngineConfig& GetConfig() const { return m_config; }

    sf::RenderWindow& GetWindow() { return *m_window; }
    const sf::RenderWindow& GetWindow() const { return *m_window; }

    LayerStack& GetLayerStack() { return m_layerStack; }
    GameStateManager& GetStateManager() { return m_stateManager; }
    GameLoop& GetGameLoop() { return m_gameLoop; }

    AppStatus GetStatus() const { return m_status; }
    bool IsRunning() const { return m_status == AppStatus::Running; }

    static Application& Instance();

private:
    void CreateWindow();
    void ProcessEvents();
    void HandleWindowEvents(sf::Event& event);

    EngineConfig m_config;
    std::unique_ptr<sf::RenderWindow> m_window;
    LayerStack m_layerStack;
    GameStateManager m_stateManager;
    GameLoop m_gameLoop;
    AppStatus m_status = AppStatus::Uninitialized;

    sf::Clock m_frameClock;
    sf::Time m_frameTime;
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_APPLICATION_H
// v2: Added window initialization
// v3: Layer and state integration
// v4: Refactored game loop into GameLoop class
// DOCS: Added inline documentation
// v5: Removed deprecated methods
// v6: C++20 compatibility
