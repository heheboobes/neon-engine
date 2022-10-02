#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/View.hpp>

namespace engine::scene {

using EntityID = uint64_t;
static constexpr EntityID INVALID_ENTITY = 0;

struct SystemBase {
    virtual ~SystemBase() = default;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual const char* name() const = 0;
};

using SystemPtr = std::unique_ptr<SystemBase>;

class Scene {
public:
    explicit Scene(const std::string& name);
    virtual ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    const std::string& getName() const { return m_name; }

    EntityID createEntity();
    void destroyEntity(EntityID id);
    bool isValid(EntityID id) const;

    template <typename T, typename... Args>
    T& addComponent(EntityID id, Args&&... args);

    template <typename T>
    T* getComponent(EntityID id);

    template <typename T>
    void removeComponent(EntityID id);

    void addSystem(SystemPtr system);
    void removeSystem(const char* name);

    void update(float dt);
    void render();

    void serialize(const std::string& path) const;
    static std::unique_ptr<Scene> deserialize(const std::string& path);

    void clear();

    sf::Color getBackgroundColor() const { return m_bgColor; }
    void setBackgroundColor(const sf::Color& color) { m_bgColor = color; }

    const sf::View& getCamera() const { return m_camera; }
    void setCamera(const sf::View& view) { m_camera = view; }

private:
    std::string m_name;
    std::vector<SystemPtr> m_systems;
    sf::Color m_bgColor;
    sf::View m_camera;
    EntityID m_nextID{ 1 };
    std::vector<bool> m_entitySlots;
};

} // namespace engine::scene
