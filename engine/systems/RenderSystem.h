#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <vector>
#include <algorithm>

namespace engine {
namespace systems {

struct RenderLayer {
    int32_t layer;
    int32_t order;
    float depth;

    RenderLayer() : layer(0), order(0), depth(0.0f) {}
    RenderLayer(int32_t l, int32_t o, float d) : layer(l), order(o), depth(d) {}
};

struct RenderCommand {
    ecs::Entity entity;
    const components::SpriteComponent* sprite;
    const components::TransformComponent* transform;
    RenderLayer layer;
};

class RenderSystem : public ecs::ISystem {
public:
    RenderSystem();
    void OnUpdate(ecs::World* world, float deltaTime) override;
    void OnRender(ecs::World* world, float alpha) override;

    void SetTarget(sf::RenderTarget* target);
    void SetViewportLayer(int32_t layer);
    void ClearColor(const sf::Color& color);
    void SetSortByDepth(bool enabled);
    void SetCamera(ecs::Entity cameraEntity);

private:
    void CollectRenderables(ecs::World* world);
    void SortRenderables();
    void SubmitDrawCalls(sf::RenderTarget& target);

    std::vector<RenderCommand> m_renderQueue;
    sf::RenderTarget* m_target;
    sf::Color m_clearColor;
    int32_t m_viewportLayer;
    bool m_sortByDepth;
    ecs::Entity m_cameraEntity;
    bool m_hasCamera;
};

} // namespace systems
} // namespace engine
// DOCS: System documentation
