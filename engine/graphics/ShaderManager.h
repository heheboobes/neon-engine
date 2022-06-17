#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace engine {
namespace graphics {

struct ShaderInfo {
    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;
    bool loaded;
    bool hasVertex;
    bool hasFragment;
    bool hasGeometry;

    ShaderInfo()
        : loaded(false)
        , hasVertex(false)
        , hasFragment(false)
        , hasGeometry(false) {}
};

class ShaderManager {
public:
    static ShaderManager& instance();

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    const sf::Shader* loadFromFile(const std::string& id,
                                   const std::string& vertexFile,
                                   const std::string& fragmentFile,
                                   const std::string& geometryFile = "");

    const sf::Shader* loadFromMemory(const std::string& id,
                                     const std::string& vertexSource,
                                     const std::string& fragmentSource,
                                     const std::string& geometrySource = "");

    const sf::Shader* get(const std::string& id) const;
    bool exists(const std::string& id) const;
    void unload(const std::string& id);
    void unloadAll();
    void reloadAll();

    ShaderInfo getInfo(const std::string& id) const;
    std::vector<std::string> getLoadedIds() const;
    std::size_t getShaderCount() const;

    bool setUniformInt(const std::string& shaderId, const std::string& name, int value);
    bool setUniformFloat(const std::string& shaderId, const std::string& name, float value);
    bool setUniformVec2(const std::string& shaderId, const std::string& name, const sf::Vector2f& value);
    bool setUniformVec3(const std::string& shaderId, const std::string& name, const sf::Vector3f& value);
    bool setUniformColor(const std::string& shaderId, const std::string& name, const sf::Color& value);
    bool setUniformTransform(const std::string& shaderId, const std::string& name, const sf::Transform& value);
    bool setUniformTexture(const std::string& shaderId, const std::string& name, const sf::Texture& texture);

    bool setCurrentUniformInt(const std::string& name, int value);
    bool setCurrentUniformFloat(const std::string& name, float value);
    bool setCurrentUniformVec2(const std::string& name, const sf::Vector2f& value);
    bool setCurrentUniformVec3(const std::string& name, const sf::Vector3f& value);
    bool setCurrentUniformColor(const std::string& name, const sf::Color& value);

    void bind(const std::string& id);
    void unbind();
    const sf::Shader* getCurrent() const;

    bool isAvailable() const;
    static bool checkHardwareSupport();

private:
    ShaderManager() = default;
    ~ShaderManager();
    void applyCurrentUniforms();

    struct ShaderEntry {
        std::unique_ptr<sf::Shader> shader;
        ShaderInfo info;
        std::unordered_map<std::string, sf::Glsl::Vec4> pendingVec4Uniforms;
        std::unordered_map<std::string, float> pendingFloatUniforms;
        std::unordered_map<std::string, int> pendingIntUniforms;
    };

    std::unordered_map<std::string, ShaderEntry> m_shaders;
    std::string m_currentShaderId;
};

} // namespace graphics
} // namespace engine
