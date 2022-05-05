#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include "../../engine/ecs/Component.h"
#include <vector>
#include <cstdint>
#include <string>

namespace engine {
namespace components {

struct NavMeshRef {
    int navMeshId;
    std::string navMeshName;
    uint32_t polygonIndex;
    uint32_t edgeIndex;

    NavMeshRef() : navMeshId(-1), polygonIndex(0), edgeIndex(0) {}
};

struct Waypoint {
    sf::Vector3f position;
    float waitTime;
    float tolerance;
    bool reached;

    Waypoint() : waitTime(0.0f), tolerance(5.0f), reached(false) {}
    explicit Waypoint(const sf::Vector3f& pos, float wait = 0.0f, float tol = 5.0f)
        : position(pos), waitTime(wait), tolerance(tol), reached(false) {}
};

enum class PathfindingState {
    Idle,
    PathCalculating,
    Moving,
    Waiting,
    PathComplete,
    Stuck,
    NoPath
};

class PathfindingComponent : public ecs::Component<PathfindingComponent> {
public:
    std::vector<Waypoint> waypoints;
    std::vector<sf::Vector3f> currentPath;

    NavMeshRef navMeshRef;
    PathfindingState state;

    float moveSpeed;
    float rotationSpeed;
    float stoppingDistance;
    float pathUpdateInterval;
    float pathUpdateTimer;

    int currentWaypointIndex;
    int currentPathIndex;

    bool pathLooped;
    bool pathReversed;
    bool avoidObstacles;
    bool smoothPath;
    bool enabled;

    sf::Vector3f targetPosition;
    float stuckTimer;
    float stuckThreshold;

    PathfindingComponent();

    void SetDestination(const sf::Vector3f& destination);
    void SetWaypoints(const std::vector<Waypoint>& wps);
    void AddWaypoint(const sf::Vector3f& pos, float waitTime = 0.0f);
    void ClearWaypoints();
    void ClearPath();

    void StartPathfinding();
    void StopPathfinding();
    void PausePathfinding();
    void ResumePathfinding();

    void SetSpeed(float speed);
    void SetStoppingDistance(float dist);
    void SetLooped(bool looped);

    Waypoint GetCurrentWaypoint() const;
    sf::Vector3f GetCurrentTarget() const;
    bool HasReachedDestination() const;

    void AdvanceToNextWaypoint();
    void RestartPath();
    void ReversePath();

    void Update(float deltaTime, sf::Vector3f& outMovement, sf::Vector3f& currentPosition);

    float GetPathProgress() const;
    int GetRemainingWaypointCount() const;
    PathfindingState GetState() const;
};

} // namespace components
} // namespace engine
