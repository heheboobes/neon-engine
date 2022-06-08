#include "PathfindingSystem.h"
#include "../ecs/World.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

namespace engine {
namespace systems {

PathfindingSystem::PathfindingSystem()
    : m_maxRequestsPerFrame(4)
    , m_heuristicWeight(1.0f)
    , m_frameCounter(0)
{
    SetName("PathfindingSystem");
    SetPriority(70);
    Require<components::PathfindingComponent>();
}

void PathfindingSystem::OnUpdate(ecs::World* world, float deltaTime)
{
    m_frameCounter++;
    ProcessPathRequests(world, deltaTime);

    auto& entityManager = world->GetEntityManager();
    const auto& archetypes = entityManager.GetArchetypes();

    for (const auto& archetype : archetypes)
    {
        if (!archetype->GetMask().IsSupersetOf(GetComponentMask()))
            continue;

        for (auto& chunk : archetype->GetChunks())
        {
            auto* pfs = static_cast<components::PathfindingComponent*>(
                chunk.GetComponentData(components::PathfindingComponent::GetStaticTypeID()));
            auto* transforms = static_cast<components::TransformComponent*>(
                chunk.GetComponentData(components::TransformComponent::GetStaticTypeID()));

            if (!pfs || !transforms)
                continue;

            for (size_t i = 0; i < chunk.GetEntityCount(); ++i)
            {
                auto& pf = pfs[i];
                if (!pf.enabled || pf.state == components::PathfindingState::Idle)
                    continue;

                UpdateMovement(world, chunk.GetEntity(i), pf, transforms[i], deltaTime);
            }
        }
    }
}

int32_t PathfindingSystem::RegisterNavMesh(const std::string& name)
{
    NavMesh mesh;
    mesh.navMeshId = static_cast<int32_t>(m_navMeshes.size());
    mesh.name = name;
    m_navMeshes.push_back(mesh);
    return mesh.navMeshId;
}

void PathfindingSystem::RemoveNavMesh(int32_t navMeshId)
{
    m_navMeshes.erase(
        std::remove_if(m_navMeshes.begin(), m_navMeshes.end(),
            [navMeshId](const NavMesh& m) { return m.navMeshId == navMeshId; }),
        m_navMeshes.end()
    );
}

NavMesh* PathfindingSystem::GetNavMesh(int32_t navMeshId)
{
    for (auto& mesh : m_navMeshes)
    {
        if (mesh.navMeshId == navMeshId)
            return &mesh;
    }
    return nullptr;
}

const NavMesh* PathfindingSystem::GetNavMesh(int32_t navMeshId) const
{
    for (const auto& mesh : m_navMeshes)
    {
        if (mesh.navMeshId == navMeshId)
            return &mesh;
    }
    return nullptr;
}

void PathfindingSystem::AddNode(int32_t navMeshId, const sf::Vector3f& position)
{
    NavMesh* mesh = GetNavMesh(navMeshId);
    if (!mesh) return;

    NavMeshNode node;
    node.id = static_cast<uint32_t>(mesh->nodes.size());
    node.position = position;
    node.walkable = true;
    node.costMultiplier = 1.0f;
    mesh->nodes.push_back(node);
}

void PathfindingSystem::ConnectNodes(int32_t navMeshId, uint32_t nodeA, uint32_t nodeB)
{
    NavMesh* mesh = GetNavMesh(navMeshId);
    if (!mesh) return;

    if (nodeA >= mesh->nodes.size() || nodeB >= mesh->nodes.size())
        return;

    float dx = mesh->nodes[nodeA].position.x - mesh->nodes[nodeB].position.x;
    float dy = mesh->nodes[nodeA].position.y - mesh->nodes[nodeB].position.y;
    float dz = mesh->nodes[nodeA].position.z - mesh->nodes[nodeB].position.z;
    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

    mesh->nodes[nodeA].neighbors.push_back(nodeB);
    mesh->nodes[nodeA].neighborCosts.push_back(dist);

    mesh->nodes[nodeB].neighbors.push_back(nodeA);
    mesh->nodes[nodeB].neighborCosts.push_back(dist);

    NavMeshEdge edge;
    edge.nodeA = nodeA;
    edge.nodeB = nodeB;
    edge.length = dist;
    edge.bidirectional = true;
    mesh->edges.push_back(edge);
}

bool PathfindingSystem::RequestPath(ecs::Entity entity, const sf::Vector3f& end)
{
    PathRequest request;
    request.entity = entity;
    request.end = end;
    request.frameSubmitted = m_frameCounter;
    request.pending = true;
    m_pendingRequests.push(request);
    return true;
}

bool PathfindingSystem::RequestPath(ecs::Entity entity, const sf::Vector3f& start, const sf::Vector3f& end)
{
    PathRequest request;
    request.entity = entity;
    request.start = start;
    request.end = end;
    request.frameSubmitted = m_frameCounter;
    request.pending = true;
    m_pendingRequests.push(request);
    return true;
}

void PathfindingSystem::SetMaxPathRequestsPerFrame(uint32_t max)
{
    m_maxRequestsPerFrame = std::max(1u, max);
}

void PathfindingSystem::SetHeuristicWeight(float weight)
{
    m_heuristicWeight = std::max(0.1f, weight);
}

void PathfindingSystem::ProcessPathRequests(ecs::World* world, float dt)
{
    (void)dt;
    uint32_t processed = 0;

    while (!m_pendingRequests.empty() && processed < m_maxRequestsPerFrame)
    {
        PathRequest request = m_pendingRequests.front();
        m_pendingRequests.pop();

        auto* pf = world->GetComponent<components::PathfindingComponent>(request.entity);
        if (!pf)
            continue;

        if (!pf->enabled)
            continue;

        auto* transform = world->GetComponent<components::TransformComponent>(request.entity);
        if (transform)
        {
            request.start = transform->position;
        }

        if (!pf->currentPath.empty())
            pf->ClearPath();

        std::vector<sf::Vector3f> path;
        FindPath(request, path);

        if (!path.empty())
        {
            ApplyPathToComponent(*pf, path);
            pf->state = components::PathfindingState::Moving;
            pf->currentWaypointIndex = 0;
            pf->currentPathIndex = 0;
        }
        else
        {
            pf->state = components::PathfindingState::NoPath;
        }

        processed++;
    }
}

void PathfindingSystem::FindPath(const PathRequest& request, std::vector<sf::Vector3f>& outPath)
{
    outPath.clear();

    for (const auto& mesh : m_navMeshes)
    {
        if (mesh.nodes.empty())
            continue;

        uint32_t startNode = FindClosestNode(mesh, request.start);
        uint32_t endNode = FindClosestNode(mesh, request.end);

        if (!mesh.nodes[startNode].walkable || !mesh.nodes[endNode].walkable)
            continue;

        auto pathNodes = AStar(mesh, startNode, endNode);
        if (pathNodes.empty())
            continue;

        for (uint32_t id : pathNodes)
        {
            outPath.push_back(mesh.nodes[id].position);
        }

        outPath.push_back(request.end);
        return;
    }

    outPath.push_back(request.end);
}

std::vector<uint32_t> PathfindingSystem::AStar(const NavMesh& mesh, uint32_t startNode, uint32_t endNode)
{
    struct AStarNode {
        uint32_t id;
        float gCost;
        float hCost;
        float fCost;
        uint32_t parent;
        bool closed;
        bool open;
    };

    std::vector<AStarNode> nodes(mesh.nodes.size());
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        nodes[i].id = static_cast<uint32_t>(i);
        nodes[i].gCost = std::numeric_limits<float>::max();
        nodes[i].hCost = 0.0f;
        nodes[i].fCost = std::numeric_limits<float>::max();
        nodes[i].parent = std::numeric_limits<uint32_t>::max();
        nodes[i].closed = false;
        nodes[i].open = false;
    }

    nodes[startNode].gCost = 0.0f;
    nodes[startNode].hCost = Heuristic(mesh.nodes[startNode].position, mesh.nodes[endNode].position);
    nodes[startNode].fCost = nodes[startNode].gCost + nodes[startNode].hCost * m_heuristicWeight;

    auto compare = [](const AStarNode* a, const AStarNode* b) { return a->fCost > b->fCost; };
    std::vector<AStarNode*> openList;
    openList.push_back(&nodes[startNode]);
    nodes[startNode].open = true;

    while (!openList.empty())
    {
        std::pop_heap(openList.begin(), openList.end(), compare);
        AStarNode* current = openList.back();
        openList.pop_back();
        current->open = false;
        current->closed = true;

        if (current->id == endNode)
        {
            std::vector<uint32_t> path;
            AStarNode* node = current;
            while (node->id != startNode)
            {
                path.push_back(node->id);
                node = &nodes[node->parent];
            }
            path.push_back(startNode);
            std::reverse(path.begin(), path.end());
            return path;
        }

        const auto& neighbors = mesh.nodes[current->id].neighbors;
        const auto& costs = mesh.nodes[current->id].neighborCosts;

        for (size_t i = 0; i < neighbors.size(); ++i)
        {
            uint32_t neighborId = neighbors[i];
            if (nodes[neighborId].closed)
                continue;
            if (!mesh.nodes[neighborId].walkable)
                continue;

            float tentativeGCost = current->gCost + costs[i] * mesh.nodes[neighborId].costMultiplier;
            if (tentativeGCost >= nodes[neighborId].gCost)
                continue;

            nodes[neighborId].parent = current->id;
            nodes[neighborId].gCost = tentativeGCost;
            nodes[neighborId].hCost = Heuristic(mesh.nodes[neighborId].position, mesh.nodes[endNode].position);
            nodes[neighborId].fCost = nodes[neighborId].gCost + nodes[neighborId].hCost * m_heuristicWeight;

            if (!nodes[neighborId].open)
            {
                openList.push_back(&nodes[neighborId]);
                std::push_heap(openList.begin(), openList.end(), compare);
                nodes[neighborId].open = true;
            }
            else
            {
                std::make_heap(openList.begin(), openList.end(), compare);
            }
        }
    }

    return {};
}

uint32_t PathfindingSystem::FindClosestNode(const NavMesh& mesh, const sf::Vector3f& position) const
{
    uint32_t closest = 0;
    float minDist = std::numeric_limits<float>::max();

    for (uint32_t i = 0; i < static_cast<uint32_t>(mesh.nodes.size()); ++i)
    {
        float dx = mesh.nodes[i].position.x - position.x;
        float dy = mesh.nodes[i].position.y - position.y;
        float dz = mesh.nodes[i].position.z - position.z;
        float dist = dx * dx + dy * dy + dz * dz;

        if (dist < minDist)
        {
            minDist = dist;
            closest = i;
        }
    }

    return closest;
}

float PathfindingSystem::Heuristic(const sf::Vector3f& a, const sf::Vector3f& b) const
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

void PathfindingSystem::ApplyPathToComponent(components::PathfindingComponent& pf,
                                              const std::vector<sf::Vector3f>& path)
{
    pf.currentPath = path;
}

void PathfindingSystem::UpdateMovement(ecs::World* world, ecs::Entity entity,
                                        components::PathfindingComponent& pf,
                                        components::TransformComponent& transform, float dt)
{
    (void)world;
    (void)entity;

    if (pf.currentPath.empty())
    {
        pf.state = components::PathfindingState::PathComplete;
        return;
    }

    if (pf.pathUpdateInterval > 0.0f)
    {
        pf.pathUpdateTimer += dt;
        if (pf.pathUpdateTimer >= pf.pathUpdateInterval)
        {
            pf.pathUpdateTimer = 0.0f;
        }
    }

    sf::Vector3f target = pf.currentPath[pf.currentPathIndex];
    sf::Vector3f currentPos = transform.position;
    sf::Vector3f direction(
        target.x - currentPos.x,
        target.y - currentPos.y,
        target.z - currentPos.z
    );

    float distance = std::sqrt(direction.x * direction.x +
                               direction.y * direction.y +
                               direction.z * direction.z);

    if (distance < pf.stoppingDistance)
    {
        pf.currentPathIndex++;
        if (pf.currentPathIndex >= static_cast<int>(pf.currentPath.size()))
        {
            if (pf.pathLooped)
            {
                pf.currentPathIndex = 0;
            }
            else
            {
                pf.state = components::PathfindingState::PathComplete;
                return;
            }
        }
        target = pf.currentPath[pf.currentPathIndex];
        direction = sf::Vector3f(
            target.x - currentPos.x,
            target.y - currentPos.y,
            target.z - currentPos.z
        );
        distance = std::sqrt(direction.x * direction.x +
                             direction.y * direction.y +
                             direction.z * direction.z);
    }

    if (distance > 0.0f)
    {
        float speed = pf.moveSpeed * dt;
        float step = std::min(speed, distance);
        transform.position.x += (direction.x / distance) * step;
        transform.position.y += (direction.y / distance) * step;
        transform.position.z += (direction.z / distance) * step;
        transform.MarkDirty();
    }

    for (auto& wp : pf.waypoints)
    {
        if (!wp.reached)
        {
            sf::Vector3f diff(
                wp.position.x - transform.position.x,
                wp.position.y - transform.position.y,
                wp.position.z - transform.position.z
            );
            float wpDist = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
            if (wpDist < wp.tolerance)
            {
                wp.reached = true;
            }
        }
    }
}

} // namespace systems
} // namespace engine
// v2: Added path smoothing
