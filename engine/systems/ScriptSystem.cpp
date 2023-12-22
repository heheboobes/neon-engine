#include "ScriptSystem.h"
#include "../ecs/World.h"
#include <algorithm>
#include <chrono>

namespace engine {
namespace systems {

ScriptSystem::ScriptSystem()
    : m_scriptRoot("./scripts/")
    , m_updateBudgetMs(5.0f)
    , m_elapsedThisFrame(0.0f)
    , m_throwOnError(false)
{
    SetName("ScriptSystem");
    SetPriority(10);
    Require<components::ScriptComponent>();
}

void ScriptSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    m_elapsedThisFrame = 0.0f;

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* scripts = static_cast<components::ScriptComponent*>(
                chunk.GetComponentData(components::ScriptComponent::GetStaticTypeID()));

            if (!scripts)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                if (m_elapsedThisFrame >= m_updateBudgetMs / 1000.0f)
                    return;

                auto& script = scripts[i];
                if (!script.enabled || script.state != components::ScriptState::Running)
                    continue;

                auto startTime = std::chrono::high_resolution_clock::now();
                UpdateScript(world, chunk.GetEntity(i), script, deltaTime);
                auto endTime = std::chrono::high_resolution_clock::now();

                float elapsed = std::chrono::duration<float>(endTime - startTime).count();
                m_elapsedThisFrame += elapsed;
            }
        }
    }
}

void ScriptSystem::OnEntityCreated(ecs::World* world, ecs::Entity entity)
{
    auto* script = world->GetComponent<components::ScriptComponent>(entity);
    if (script)
    {
        InitializeScript(world, entity, *script);
    }
}

void ScriptSystem::OnEntityDestroyed(ecs::World* world, ecs::Entity entity)
{
    auto* script = world->GetComponent<components::ScriptComponent>(entity);
    if (script)
    {
        FinalizeScript(*script);
    }
}

void ScriptSystem::ReloadAll()
{
}

void ScriptSystem::ReloadScript(const std::string& scriptName)
{
    (void)scriptName;
}

void ScriptSystem::SendEvent(const std::string& eventName, const std::any& data)
{
    (void)eventName;
    (void)data;
}

void ScriptSystem::SetScriptRoot(const std::string& path)
{
    m_scriptRoot = path;
}

void ScriptSystem::SetUpdateBudget(float milliseconds)
{
    m_updateBudgetMs = std::max(0.0f, milliseconds);
}

float ScriptSystem::GetUpdateBudget() const
{
    return m_updateBudgetMs;
}

void ScriptSystem::InitializeScript(ecs::World* world, ecs::Entity entity,
                                     components::ScriptComponent& script)
{
    (void)world;
    (void)entity;
    script.state = components::ScriptState::Running;

    for (auto& callback : script.updateCallbacks)
    {
        (void)callback;
    }
}

void ScriptSystem::UpdateScript(ecs::World* world, ecs::Entity entity,
                                 components::ScriptComponent& script, float dt)
{
    (void)world;
    (void)entity;

    if (script.updateInterval > 0.0f)
    {
        script.updateTimer += dt;
        if (script.updateTimer < script.updateInterval)
            return;
        script.updateTimer = 0.0f;
    }

    for (auto& callback : script.updateCallbacks)
    {
        try
        {
            callback(dt);
        }
        catch (...)
        {
            script.state = components::ScriptState::Error;
            if (m_throwOnError)
                throw;
        }
    }
}

void ScriptSystem::FinalizeScript(components::ScriptComponent& script)
{
    script.state = components::ScriptState::Stopped;
    script.ClearCallbacks();
}

void ScriptSystem::DispatchEventToEntity(components::ScriptComponent& script,
                                          const std::string& eventName,
                                          const std::any& data)
{
    for (auto& callback : script.eventCallbacks)
    {
        callback(eventName, data);
    }
}

} // namespace systems
} // namespace engine

