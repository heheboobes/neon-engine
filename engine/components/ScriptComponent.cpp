#include "ScriptComponent.h"
#include <fstream>
#include <sstream>

namespace engine {
namespace components {

ScriptComponent::ScriptComponent()
    : scriptName("UntitledScript")
    , state(ScriptState::Uninitialized)
    , updateInterval(0.0f)
    , updateTimer(0.0f)
    , enabled(true)
    , reloadOnChange(false)
{
}

void ScriptComponent::SetScript(const std::string& name, const std::string& source)
{
    scriptName = name;
    scriptSource = source;
    state = ScriptState::Uninitialized;
}

void ScriptComponent::LoadFromFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        scriptSource = ss.str();
        scriptPath = filepath;
        file.close();

        size_t lastSlash = filepath.find_last_of("/\\");
        size_t lastDot = filepath.find_last_of('.');
        if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
            scriptName = filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else {
            scriptName = filepath;
        }
        state = ScriptState::Uninitialized;
    }
}

void ScriptComponent::Reload()
{
    if (!scriptPath.empty()) {
        LoadFromFile(scriptPath);
    }
}

void ScriptComponent::SetProperty(const std::string& key, const std::any& value)
{
    properties[key] = value;
}

bool ScriptComponent::HasProperty(const std::string& key) const
{
    return properties.find(key) != properties.end();
}

void ScriptComponent::AddUpdateCallback(ScriptUpdateCallback callback)
{
    if (callback) {
        updateCallbacks.push_back(std::move(callback));
    }
}

void ScriptComponent::AddEventCallback(ScriptEventCallback callback)
{
    if (callback) {
        eventCallbacks.push_back(std::move(callback));
    }
}

void ScriptComponent::ClearCallbacks()
{
    updateCallbacks.clear();
    eventCallbacks.clear();
}

void ScriptComponent::Run()
{
    state = ScriptState::Running;
    updateTimer = 0.0f;
}

void ScriptComponent::Pause()
{
    if (state == ScriptState::Running) {
        state = ScriptState::Paused;
    }
}

void ScriptComponent::Stop()
{
    state = ScriptState::Stopped;
    updateTimer = 0.0f;
}

void ScriptComponent::Reset()
{
    state = ScriptState::Uninitialized;
    updateTimer = 0.0f;
    properties.clear();
}

void ScriptComponent::SendEvent(const std::string& eventName, const std::any& data)
{
    for (auto& cb : eventCallbacks) {
        cb(eventName, data);
    }
}

void ScriptComponent::Update(float deltaTime)
{
    if (!enabled || state != ScriptState::Running) return;

    if (updateInterval > 0.0f) {
        updateTimer += deltaTime;
        if (updateTimer < updateInterval) return;
        deltaTime = updateTimer;
        updateTimer = 0.0f;
    }

    for (auto& cb : updateCallbacks) {
        cb(deltaTime);
    }
}

ScriptState ScriptComponent::GetState() const
{
    return state;
}

bool ScriptComponent::IsRunning() const
{
    return state == ScriptState::Running;
}

void ScriptComponent::SetUpdateInterval(float interval)
{
    updateInterval = std::max(0.0f, interval);
    if (updateInterval > 0.0f) {
        updateTimer = 0.0f;
    }
}

} // namespace components
} // namespace engine
