#include "ShaderManager.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace engine {
namespace graphics {

ShaderManager& ShaderManager::instance() {
    static ShaderManager inst;
    return inst;
}

ShaderManager::~ShaderManager() {
    m_shaders.clear();
}

const sf::Shader* ShaderManager::loadFromFile(const std::string& id,
                                              const std::string& vertexFile,
                                              const std::string& fragmentFile,
                                              const std::string& geometryFile) {
    auto it = m_shaders.find(id);
    if (it != m_shaders.end()) {
        return it->second.shader.get();
    }

    if (!sf::Shader::isAvailable()) return nullptr;

    ShaderEntry entry;
    entry.shader = std::make_unique<sf::Shader>();
    bool success = false;

    if (!geometryFile.empty() && std::ifstream(geometryFile).good()) {
        success = entry.shader->loadFromFile(vertexFile, fragmentFile, geometryFile);
        entry.info.hasGeometry = true;
    } else {
        if (!vertexFile.empty() && !fragmentFile.empty()) {
            success = entry.shader->loadFromFile(vertexFile, fragmentFile);
            entry.info.hasVertex = true;
            entry.info.hasFragment = true;
        } else if (!vertexFile.empty()) {
            success = entry.shader->loadFromFile(vertexFile, sf::Shader::Vertex);
            entry.info.hasVertex = true;
        } else if (!fragmentFile.empty()) {
            success = entry.shader->loadFromFile(fragmentFile, sf::Shader::Fragment);
            entry.info.hasFragment = true;
        }
    }

    if (!success) {
        entry.shader.reset();
        return nullptr;
    }

    entry.info.vertexPath = vertexFile;
    entry.info.fragmentPath = fragmentFile;
    entry.info.geometryPath = geometryFile;
    entry.info.loaded = true;

    m_shaders[id] = std::move(entry);
    return m_shaders[id].shader.get();
}

const sf::Shader* ShaderManager::loadFromMemory(const std::string& id,
                                                const std::string& vertexSource,
                                                const std::string& fragmentSource,
                                                const std::string& geometrySource) {
    auto it = m_shaders.find(id);
    if (it != m_shaders.end()) {
        return it->second.shader.get();
    }

    if (!sf::Shader::isAvailable()) return nullptr;

    ShaderEntry entry;
    entry.shader = std::make_unique<sf::Shader>();
    bool success = false;

    if (!geometrySource.empty()) {
        success = entry.shader->loadFromMemory(vertexSource, fragmentSource, geometrySource);
        entry.info.hasGeometry = true;
    } else if (!vertexSource.empty() && !fragmentSource.empty()) {
        success = entry.shader->loadFromMemory(vertexSource, fragmentSource);
        entry.info.hasVertex = true;
        entry.info.hasFragment = true;
    } else if (!vertexSource.empty()) {
        success = entry.shader->loadFromMemory(vertexSource, sf::Shader::Vertex);
        entry.info.hasVertex = true;
    } else if (!fragmentSource.empty()) {
        success = entry.shader->loadFromMemory(fragmentSource, sf::Shader::Fragment);
        entry.info.hasFragment = true;
    }

    if (!success) return nullptr;

    entry.info.loaded = true;
    m_shaders[id] = std::move(entry);
    return m_shaders[id].shader.get();
}

const sf::Shader* ShaderManager::get(const std::string& id) const {
    auto it = m_shaders.find(id);
    if (it != m_shaders.end() && it->second.info.loaded) {
        return it->second.shader.get();
    }
    return nullptr;
}

bool ShaderManager::exists(const std::string& id) const {
    return m_shaders.find(id) != m_shaders.end();
}

void ShaderManager::unload(const std::string& id) {
    m_shaders.erase(id);
    if (m_currentShaderId == id) {
        m_currentShaderId.clear();
    }
}

void ShaderManager::unloadAll() {
    m_shaders.clear();
    m_currentShaderId.clear();
}

void ShaderManager::reloadAll() {
    for (auto& pair : m_shaders) {
        auto& entry = pair.second;
        const auto& info = entry.info;
        if (!info.loaded) continue;

        auto newShader = std::make_unique<sf::Shader>();
        bool success = false;

        if (info.hasGeometry && !info.geometryPath.empty()) {
            success = newShader->loadFromFile(info.vertexPath, info.fragmentPath, info.geometryPath);
        } else if (info.hasVertex && info.hasFragment) {
            success = newShader->loadFromFile(info.vertexPath, info.fragmentPath);
        } else if (info.hasVertex) {
            success = newShader->loadFromFile(info.vertexPath, sf::Shader::Vertex);
        } else if (info.hasFragment) {
            success = newShader->loadFromFile(info.fragmentPath, sf::Shader::Fragment);
        }

        if (success) {
            entry.shader = std::move(newShader);
        }
    }
}

ShaderInfo ShaderManager::getInfo(const std::string& id) const {
    auto it = m_shaders.find(id);
    if (it != m_shaders.end()) {
        return it->second.info;
    }
    return ShaderInfo();
}

std::vector<std::string> ShaderManager::getLoadedIds() const {
    std::vector<std::string> ids;
    ids.reserve(m_shaders.size());
    for (const auto& pair : m_shaders) {
        if (pair.second.info.loaded) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

std::size_t ShaderManager::getShaderCount() const {
    return m_shaders.size();
}

bool ShaderManager::setUniformInt(const std::string& shaderId, const std::string& name, int value) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    it->second.shader->setUniform(name, value);
    return true;
}

bool ShaderManager::setUniformFloat(const std::string& shaderId, const std::string& name, float value) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    it->second.shader->setUniform(name, value);
    return true;
}

bool ShaderManager::setUniformVec2(const std::string& shaderId, const std::string& name, const sf::Vector2f& value) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    it->second.shader->setUniform(name, value);
    return true;
}

bool ShaderManager::setUniformVec3(const std::string& shaderId, const std::string& name, const sf::Vector3f& value) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    it->second.shader->setUniform(name, value);
    return true;
}

bool ShaderManager::setUniformColor(const std::string& shaderId, const std::string& name, const sf::Color& value) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    sf::Glsl::Vec4 vec(value.r / 255.f, value.g / 255.f, value.b / 255.f, value.a / 255.f);
    it->second.shader->setUniform(name, vec);
    return true;
}

bool ShaderManager::setUniformTransform(const std::string& shaderId, const std::string& name, const sf::Transform& value) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    it->second.shader->setUniform(name, sf::Glsl::Mat4(static_cast<const float*>(value.getMatrix())));
    return true;
}

bool ShaderManager::setUniformTexture(const std::string& shaderId, const std::string& name, const sf::Texture& texture) {
    auto it = m_shaders.find(shaderId);
    if (it == m_shaders.end() || !it->second.shader) return false;
    it->second.shader->setUniform(name, texture);
    return true;
}

bool ShaderManager::setCurrentUniformInt(const std::string& name, int value) {
    if (m_currentShaderId.empty()) return false;
    return setUniformInt(m_currentShaderId, name, value);
}

bool ShaderManager::setCurrentUniformFloat(const std::string& name, float value) {
    if (m_currentShaderId.empty()) return false;
    return setUniformFloat(m_currentShaderId, name, value);
}

bool ShaderManager::setCurrentUniformVec2(const std::string& name, const sf::Vector2f& value) {
    if (m_currentShaderId.empty()) return false;
    return setUniformVec2(m_currentShaderId, name, value);
}

bool ShaderManager::setCurrentUniformVec3(const std::string& name, const sf::Vector3f& value) {
    if (m_currentShaderId.empty()) return false;
    return setUniformVec3(m_currentShaderId, name, value);
}

bool ShaderManager::setCurrentUniformColor(const std::string& name, const sf::Color& value) {
    if (m_currentShaderId.empty()) return false;
    return setUniformColor(m_currentShaderId, name, value);
}

void ShaderManager::bind(const std::string& id) {
    auto it = m_shaders.find(id);
    if (it != m_shaders.end() && it->second.shader) {
        sf::Shader::bind(it->second.shader.get());
        m_currentShaderId = id;
        applyCurrentUniforms();
    }
}

void ShaderManager::unbind() {
    sf::Shader::bind(nullptr);
    m_currentShaderId.clear();
}

const sf::Shader* ShaderManager::getCurrent() const {
    auto it = m_shaders.find(m_currentShaderId);
    return (it != m_shaders.end()) ? it->second.shader.get() : nullptr;
}

bool ShaderManager::isAvailable() const {
    return sf::Shader::isAvailable();
}

bool ShaderManager::checkHardwareSupport() {
    return sf::Shader::isAvailable();
}

void ShaderManager::applyCurrentUniforms() {
    auto it = m_shaders.find(m_currentShaderId);
    if (it == m_shaders.end() || !it->second.shader) return;

    auto& entry = it->second;
    for (const auto& pair : entry.pendingFloatUniforms) {
        entry.shader->setUniform(pair.first, pair.second);
    }
    for (const auto& pair : entry.pendingIntUniforms) {
        entry.shader->setUniform(pair.first, pair.second);
    }
    for (const auto& pair : entry.pendingVec4Uniforms) {
        entry.shader->setUniform(pair.first, pair.second);
    }
}

} // namespace graphics
} // namespace engine
// v2: Uniform caching bug
