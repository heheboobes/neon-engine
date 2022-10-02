#include "Scene.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace engine::scene {

Scene::Scene(const std::string& name)
    : m_name(name)
    , m_bgColor(sf::Color::Black)
    , m_camera(sf::FloatRect(0.f, 0.f, 1920.f, 1080.f))
{
    m_entitySlots.reserve(1024);
}

Scene::~Scene() = default;

EntityID Scene::createEntity()
{
    for (size_t i = 0; i < m_entitySlots.size(); ++i) {
        if (!m_entitySlots[i]) {
            m_entitySlots[i] = true;
            return static_cast<EntityID>(i + 1);
        }
    }
    m_entitySlots.push_back(true);
    return static_cast<EntityID>(m_entitySlots.size());
}

void Scene::destroyEntity(EntityID id)
{
    if (id == INVALID_ENTITY || id > m_entitySlots.size())
        return;
    m_entitySlots[id - 1] = false;
}

bool Scene::isValid(EntityID id) const
{
    if (id == INVALID_ENTITY || id > m_entitySlots.size())
        return false;
    return m_entitySlots[id - 1];
}

void Scene::addSystem(SystemPtr system)
{
    auto it = std::find_if(m_systems.begin(), m_systems.end(),
        [&](const auto& s) { return std::strcmp(s->name(), system->name()) == 0; });
    if (it != m_systems.end())
        return;
    m_systems.push_back(std::move(system));
}

void Scene::removeSystem(const char* name)
{
    auto it = std::remove_if(m_systems.begin(), m_systems.end(),
        [name](const auto& s) { return std::strcmp(s->name(), name) == 0; });
    m_systems.erase(it, m_systems.end());
}

void Scene::update(float dt)
{
    for (auto& sys : m_systems)
        sys->update(dt);
}

void Scene::render()
{
    for (auto& sys : m_systems)
        sys->render();
}

void Scene::serialize(const std::string& path) const
{
    std::ofstream out(path);
    if (!out.is_open())
        return;
    out << "scene_name:" << m_name << "\n";
    out << "bg_r:" << static_cast<int>(m_bgColor.r) << "\n";
    out << "bg_g:" << static_cast<int>(m_bgColor.g) << "\n";
    out << "bg_b:" << static_cast<int>(m_bgColor.b) << "\n";
    out << "bg_a:" << static_cast<int>(m_bgColor.a) << "\n";
    out << "entity_count:" << m_entitySlots.size() << "\n";
}

std::unique_ptr<Scene> Scene::deserialize(const std::string& path)
{
    std::ifstream in(path);
    if (!in.is_open())
        return nullptr;

    auto scene = std::make_unique<Scene>("unnamed");
    std::string line;
    while (std::getline(in, line)) {
        auto sep = line.find(':');
        if (sep == std::string::npos) continue;
        auto key = line.substr(0, sep);
        auto val = line.substr(sep + 1);
        if (key == "scene_name") scene->m_name = val;
        else if (key == "bg_r") scene->m_bgColor.r = static_cast<uint8_t>(std::stoi(val));
        else if (key == "bg_g") scene->m_bgColor.g = static_cast<uint8_t>(std::stoi(val));
        else if (key == "bg_b") scene->m_bgColor.b = static_cast<uint8_t>(std::stoi(val));
        else if (key == "bg_a") scene->m_bgColor.a = static_cast<uint8_t>(std::stoi(val));
    }
    return scene;
}

void Scene::clear()
{
    m_systems.clear();
    m_entitySlots.clear();
    m_nextID = 1;
}

} // namespace engine::scene
