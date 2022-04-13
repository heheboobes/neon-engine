#pragma once

#include <SFML/System/Time.hpp>
#include "../../engine/ecs/Component.h"
#include <string>
#include <unordered_map>
#include <any>
#include <vector>
#include <functional>

namespace engine {
namespace components {

using ScriptPropertyTable = std::unordered_map<std::string, std::any>;
using ScriptUpdateCallback = std::function<void(float)>;
using ScriptEventCallback = std::function<void(const std::string&, const std::any&)>;

enum class ScriptState {
    Uninitialized,
    Running,
    Paused,
    Stopped,
    Error
};

class ScriptComponent : public ecs::Component<ScriptComponent> {
public:
    std::string scriptName;
    std::string scriptSource;
    std::string scriptPath;

    ScriptPropertyTable properties;
    ScriptState state;

    std::vector<ScriptUpdateCallback> updateCallbacks;
    std::vector<ScriptEventCallback> eventCallbacks;

    float updateInterval;
    float updateTimer;

    bool enabled;
    bool reloadOnChange;

    ScriptComponent();

    void SetScript(const std::string& name, const std::string& source);
    void LoadFromFile(const std::string& filepath);
    void Reload();

    void SetProperty(const std::string& key, const std::any& value);
    template<typename T>
    T GetProperty(const std::string& key, const T& defaultValue = T()) const
    {
        auto it = properties.find(key);
        if (it != properties.end()) {
            try { return std::any_cast<T>(it->second); }
            catch (const std::bad_any_cast&) {}
        }
        return defaultValue;
    }
    bool HasProperty(const std::string& key) const;

    void AddUpdateCallback(ScriptUpdateCallback callback);
    void AddEventCallback(ScriptEventCallback callback);
    void ClearCallbacks();

    void Run();
    void Pause();
    void Stop();
    void Reset();

    void SendEvent(const std::string& eventName, const std::any& data = std::any());
    void Update(float deltaTime);

    ScriptState GetState() const;
    bool IsRunning() const;

    void SetUpdateInterval(float interval);
};

} // namespace components
} // namespace engine
