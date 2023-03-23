#include "EngineConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace engine {
namespace core {

EngineConfig EngineConfig::CreateDefault()
{
    EngineConfig config;
    config.video = VideoSettings();
    config.audio = AudioSettings();
    config.physics = PhysicsSettings();
    config.rendering = RenderingSettings();
    config.assetRoot = "assets/";
    config.saveDataPath = "saves/";
    config.logFilePath = "logs/engine.log";
    config.threadPoolSize = std::max(1u, std::thread::hardware_concurrency());
    config.enableProfiling = false;
    return config;
}

EngineConfig EngineConfig::FromFile(const std::string& filepath)
{
    EngineConfig config = CreateDefault();
    std::ifstream file(filepath);
    if (!file.is_open())
        return config;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        auto eqPos = line.find('=');
        if (eqPos == std::string::npos)
            continue;

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (key == "width") config.video.width = std::stoul(value);
        else if (key == "height") config.video.height = std::stoul(value);
        else if (key == "fullscreen") config.video.fullscreen = (value == "true");
        else if (key == "vsync") config.video.vsyncEnabled = (value == "true");
        else if (key == "max_fps") config.video.maxFPS = std::stoul(value);
        else if (key == "master_volume") config.audio.masterVolume = std::stof(value);
        else if (key == "music_volume") config.audio.musicVolume = std::stof(value);
        else if (key == "sfx_volume") config.audio.sfxVolume = std::stof(value);
        else if (key == "fixed_timestep") config.physics.fixedTimestep = std::stof(value);
        else if (key == "gravity_x") config.physics.gravityX = std::stof(value);
        else if (key == "gravity_y") config.physics.gravityY = std::stof(value);
        else if (key == "debug_draw") config.rendering.debugDraw = (value == "true");
        else if (key == "show_fps") config.rendering.showFPS = (value == "true");
        else if (key == "window_title") config.rendering.windowTitle = value;
        else if (key == "asset_root") config.assetRoot = value;
        else if (key == "thread_pool_size") config.threadPoolSize = std::stoul(value);
    }

    config.Validate();
    return config;
}

bool EngineConfig::SaveToFile(const std::string& filepath) const
{
    std::ofstream file(filepath);
    if (!file.is_open())
        return false;

    file << "# Engine Configuration\n";
    file << "width=" << video.width << "\n";
    file << "height=" << video.height << "\n";
    file << "fullscreen=" << (video.fullscreen ? "true" : "false") << "\n";
    file << "vsync=" << (video.vsyncEnabled ? "true" : "false") << "\n";
    file << "max_fps=" << video.maxFPS << "\n";
    file << "master_volume=" << audio.masterVolume << "\n";
    file << "music_volume=" << audio.musicVolume << "\n";
    file << "sfx_volume=" << audio.sfxVolume << "\n";
    file << "fixed_timestep=" << physics.fixedTimestep << "\n";
    file << "gravity_x=" << physics.gravityX << "\n";
    file << "gravity_y=" << physics.gravityY << "\n";
    file << "debug_draw=" << (rendering.debugDraw ? "true" : "false") << "\n";
    file << "show_fps=" << (rendering.showFPS ? "true" : "false") << "\n";
    file << "window_title=" << rendering.windowTitle << "\n";
    file << "asset_root=" << assetRoot << "\n";
    file << "thread_pool_size=" << threadPoolSize << "\n";
    return file.good();
}

void EngineConfig::Validate()
{
    video.width = std::clamp(video.width, 640u, 7680u);
    video.height = std::clamp(video.height, 480u, 4320u);
    video.maxFPS = std::clamp(video.maxFPS, 30u, 360u);
    audio.masterVolume = std::clamp(audio.masterVolume, 0.0f, 1.0f);
    audio.musicVolume = std::clamp(audio.musicVolume, 0.0f, 1.0f);
    audio.sfxVolume = std::clamp(audio.sfxVolume, 0.0f, 1.0f);
    physics.fixedTimestep = std::max(physics.fixedTimestep, 0.001f);
    physics.maxSubSteps = std::clamp(physics.maxSubSteps, 1u, 20u);
    threadPoolSize = std::max(1u, threadPoolSize);

    if (assetRoot.empty()) assetRoot = "assets/";
    if (saveDataPath.empty()) saveDataPath = "saves/";
    if (rendering.windowTitle.empty()) rendering.windowTitle = "Game Engine";
}

} // namespace core
} // namespace engine
// v2: Added runtime reload
