#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/ScriptComponent.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <any>

namespace engine {
namespace systems {

class ScriptSystem : public ecs::ISystem {
public:
    ScriptSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnEntityCreated(ecs::World* world, ecs::Entity entity) override;
    void OnEntityDestroyed(ecs::World* world, ecs::Entity entity) override;

    void ReloadAll();
    void ReloadScript(const std::string& scriptName);
    void SendEvent(const std::string& eventName, const std::any& data = std::any());
    void SetScriptRoot(const std::string& path);

    void SetUpdateBudget(float milliseconds);
    float GetUpdateBudget() const;

private:
    void InitializeScript(ecs::World* world, ecs::Entity entity,
                          components::ScriptComponent& script);
    void UpdateScript(ecs::World* world, ecs::Entity entity,
                      components::ScriptComponent& script, float dt);
    void FinalizeScript(components::ScriptComponent& script);
    void DispatchEventToEntity(components::ScriptComponent& script,
                               const std::string& eventName,
                               const std::any& data);

    std::string m_scriptRoot;
    float m_updateBudgetMs;
    float m_elapsedThisFrame;
    bool m_throwOnError;
};

} // namespace systems
} // namespace engine
