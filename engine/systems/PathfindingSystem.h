#pragma once

#include "../ecs/System.h"
#include "../ecs/World.h"
#include "../components/PathfindingComponent.h"
#include "../components/TransformComponent.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <queue>

namespace engine {
namespace systems {

struct NavMeshNode {
    uint32_t id;
    sf::Vector3f position;
    std::vector<uint32_t> neighbors;
    std::vector<float> neighborCosts;
    bool walkable;
    float costMultiplier;
};

struct NavMeshEdge {
    uint32_t nodeA;
    uint32_t nodeB;
    float length;
    bool bidirectional;
};

struct NavMesh {
    int32_t navMeshId;
    std::string name;
    std::vector<NavMeshNode> nodes;
    std::vector<NavMeshEdge> edges;
    sf::Vector3f boundsMin;
    sf::Vector3f boundsMax;
};

struct PathRequest {
    ecs::Entity entity;
    sf::Vector3f start;
    sf::Vector3f end;
    uint32_t frameSubmitted;
    bool pending;
};

class PathfindingSystem : public ecs::ISystem {
public:
    PathfindingSystem();

    void OnUpdate(ecs::World* world, float deltaTime) override;

    int32_t RegisterNavMesh(const std::string& name);
    void RemoveNavMesh(int32_t navMeshId);
    NavMesh* GetNavMesh(int32_t navMeshId);
    const NavMesh* GetNavMesh(int32_t navMeshId) const;

    void AddNode(int32_t navMeshId, const sf::Vector3f& position);
    void ConnectNodes(int32_t navMeshId, uint32_t nodeA, uint32_t nodeB);

    bool RequestPath(ecs::Entity entity, const sf::Vector3f& end);
    bool RequestPath(ecs::Entity entity, const sf::Vector3f& start, const sf::Vector3f& end);

    void SetMaxPathRequestsPerFrame(uint32_t max);
    void SetHeuristicWeight(float weight);

private:
    struct AStarNode {
        uint32_t id;
        float gCost;
        float hCost;
        float fCost;
        uint32_t parent;
        bool closed;
        bool open;
    };

    void ProcessPathRequests(ecs::World* world, float dt);
    void FindPath(const PathRequest& request, std::vector<sf::Vector3f>& outPath);
    std::vector<uint32_t> AStar(const NavMesh& mesh, uint32_t startNode, uint32_t endNode);
    uint32_t FindClosestNode(const NavMesh& mesh, const sf::Vector3f& position) const;
    float Heuristic(const sf::Vector3f& a, const sf::Vector3f& b) const;
    void ApplyPathToComponent(components::PathfindingComponent& pf, const std::vector<sf::Vector3f>& path);
    void UpdateMovement(ecs::World* world, ecs::Entity entity,
                        components::PathfindingComponent& pf,
                        components::TransformComponent& transform, float dt);

    std::vector<NavMesh> m_navMeshes;
    std::queue<PathRequest> m_pendingRequests;
    uint32_t m_maxRequestsPerFrame;
    float m_heuristicWeight;
    uint32_t m_frameCounter;
};

} // namespace systems
} // namespace engine
// v2: Path smoothing
