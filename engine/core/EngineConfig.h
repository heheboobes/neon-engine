#ifndef ENGINE_CORE_ENGINE_CONFIG_H
#define ENGINE_CORE_ENGINE_CONFIG_H

#include <string>
#include <cstdint>

namespace engine {
namespace core {

struct VideoSettings {
    uint32_t width = 1280;
    uint32_t height = 720;
    uint32_t bitsPerPixel = 32;
    bool fullscreen = false;
    bool vsyncEnabled = true;
    uint32_t maxFPS = 144;
};

struct AudioSettings {
    float masterVolume = 1.0f;
    float musicVolume = 0.8f;
    float sfxVolume = 1.0f;
    uint32_t sampleRate = 44100;
    uint8_t audioChannels = 2;
};

struct PhysicsSettings {
    float fixedTimestep = 1.0f / 60.0f;
    uint32_t maxSubSteps = 5;
    float gravityX = 0.0f;
    float gravityY = 981.0f;
};

struct RenderingSettings {
    bool debugDraw = false;
    bool showFPS = true;
    float renderScale = 1.0f;
    std::string windowTitle = "Game Engine";
    std::string iconPath = "";
};

struct EngineConfig {
    VideoSettings video;
    AudioSettings audio;
    PhysicsSettings physics;
    RenderingSettings rendering;

    std::string assetRoot = "assets/";
    std::string saveDataPath = "saves/";
    std::string logFilePath = "logs/engine.log";
    uint32_t threadPoolSize = 4;
    bool enableProfiling = false;

    static EngineConfig CreateDefault();
    static EngineConfig FromFile(const std::string& filepath);
    bool SaveToFile(const std::string& filepath) const;
    void Validate();
};

} // namespace core
} // namespace engine

#endif // ENGINE_CORE_ENGINE_CONFIG_H
