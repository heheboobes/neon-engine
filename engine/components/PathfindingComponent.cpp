#include "PathfindingComponent.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace engine {
namespace components {

PathfindingComponent::PathfindingComponent()
    : state(PathfindingState::Idle)
    , moveSpeed(100.0f)
    , rotationSpeed(180.0f)
    , stoppingDistance(5.0f)
    , pathUpdateInterval(0.5f)
    , pathUpdateTimer(0.0f)
    , currentWaypointIndex(0)
    , currentPathIndex(0)
    , pathLooped(false)
    , pathReversed(false)
    , avoidObstacles(true)
    , smoothPath(true)
    , enabled(true)
    , targetPosition(0.f, 0.f, 0.f)
    , stuckTimer(0.0f)
    , stuckThreshold(2.0f)
{
}

void PathfindingComponent::SetDestination(const sf::Vector3f& destination)
{
    targetPosition = destination;
    state = PathfindingState::PathCalculating;
}

void PathfindingComponent::SetWaypoints(const std::vector<Waypoint>& wps)
{
    waypoints = wps;
    currentWaypointIndex = 0;
    currentPathIndex = 0;
    for (auto& wp : waypoints) {
        wp.reached = false;
    }
}

void PathfindingComponent::AddWaypoint(const sf::Vector3f& pos, float waitTime)
{
    waypoints.emplace_back(pos, waitTime);
}

void PathfindingComponent::ClearWaypoints()
{
    waypoints.clear();
    currentWaypointIndex = 0;
}

void PathfindingComponent::ClearPath()
{
    currentPath.clear();
    currentPathIndex = 0;
}

void PathfindingComponent::StartPathfinding()
{
    if (waypoints.empty() && currentPath.empty()) return;
    state = PathfindingState::Moving;
    currentWaypointIndex = 0;
    currentPathIndex = 0;
    stuckTimer = 0.0f;
}

void PathfindingComponent::StopPathfinding()
{
    state = PathfindingState::Idle;
    currentWaypointIndex = 0;
    currentPathIndex = 0;
    stuckTimer = 0.0f;
}

void PathfindingComponent::PausePathfinding()
{
    if (state == PathfindingState::Moving) {
        state = PathfindingState::Waiting;
    }
}

void PathfindingComponent::ResumePathfinding()
{
    if (state == PathfindingState::Waiting) {
        state = PathfindingState::Moving;
    }
}

void PathfindingComponent::SetSpeed(float speed)
{
    moveSpeed = std::max(0.0f, speed);
}

void PathfindingComponent::SetStoppingDistance(float dist)
{
    stoppingDistance = std::max(0.1f, dist);
}

void PathfindingComponent::SetLooped(bool looped)
{
    pathLooped = looped;
}

Waypoint PathfindingComponent::GetCurrentWaypoint() const
{
    if (waypoints.empty()) return Waypoint();
    if (currentWaypointIndex < 0 || currentWaypointIndex >= static_cast<int>(waypoints.size())) {
        return waypoints.back();
    }
    return waypoints[currentWaypointIndex];
}

sf::Vector3f PathfindingComponent::GetCurrentTarget() const
{
    if (!currentPath.empty() && currentPathIndex < static_cast<int>(currentPath.size())) {
        return currentPath[currentPathIndex];
    }
    if (!waypoints.empty() && currentWaypointIndex < static_cast<int>(waypoints.size())) {
        return waypoints[currentWaypointIndex].position;
    }
    return targetPosition;
}

bool PathfindingComponent::HasReachedDestination() const
{
    if (waypoints.empty()) return true;
    return currentWaypointIndex >= static_cast<int>(waypoints.size());
}

void PathfindingComponent::AdvanceToNextWaypoint()
{
    if (waypoints.empty()) return;

    if (currentWaypointIndex < static_cast<int>(waypoints.size())) {
        waypoints[currentWaypointIndex].reached = true;
    }

    currentWaypointIndex++;
    currentPathIndex = 0;
    currentPath.clear();

    if (currentWaypointIndex >= static_cast<int>(waypoints.size())) {
        if (pathLooped) {
            currentWaypointIndex = 0;
            for (auto& wp : waypoints) wp.reached = false;
            state = PathfindingState::Moving;
        } else {
            state = PathfindingState::PathComplete;
        }
    } else {
        Waypoint& next = waypoints[currentWaypointIndex];
        if (next.waitTime > 0.0f) {
            state = PathfindingState::Waiting;
        } else {
            state = PathfindingState::Moving;
        }
    }
}

void PathfindingComponent::RestartPath()
{
    currentWaypointIndex = 0;
    currentPathIndex = 0;
    currentPath.clear();
    stuckTimer = 0.0f;
    for (auto& wp : waypoints) wp.reached = false;
    state = waypoints.empty() ? PathfindingState::Idle : PathfindingState::Moving;
}

void PathfindingComponent::ReversePath()
{
    pathReversed = !pathReversed;
    std::reverse(waypoints.begin(), waypoints.end());
    currentWaypointIndex = 0;
    currentPathIndex = 0;
    for (auto& wp : waypoints) wp.reached = false;
}

void PathfindingComponent::Update(float deltaTime, sf::Vector3f& outMovement, sf::Vector3f& currentPosition)
{
    outMovement = sf::Vector3f(0.f, 0.f, 0.f);

    if (!enabled || state == PathfindingState::Idle || state == PathfindingState::PathComplete) return;

    if (state == PathfindingState::PathCalculating) {
        state = PathfindingState::Moving;
        return;
    }

    if (state == PathfindingState::Waiting) {
        Waypoint& wp = waypoints[currentWaypointIndex];
        wp.waitTime = std::max(0.0f, wp.waitTime - deltaTime);
        if (wp.waitTime <= 0.0f) {
            AdvanceToNextWaypoint();
        }
        return;
    }

    if (state != PathfindingState::Moving) return;

    sf::Vector3f target = GetCurrentTarget();
    sf::Vector3f diff(
        target.x - currentPosition.x,
        target.y - currentPosition.y,
        target.z - currentPosition.z
    );

    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    if (dist <= stoppingDistance) {
        if (!currentPath.empty()) {
            currentPathIndex++;
            if (currentPathIndex >= static_cast<int>(currentPath.size())) {
                currentPath.clear();
                currentPathIndex = 0;
                AdvanceToNextWaypoint();
            }
        } else {
            AdvanceToNextWaypoint();
        }
        stuckTimer = 0.0f;
        return;
    }

    float speed = moveSpeed * deltaTime;
    float factor = std::min(1.0f, speed / dist);
    outMovement = sf::Vector3f(diff.x * factor, diff.y * factor, diff.z * factor);

    stuckTimer += deltaTime;
    if (stuckTimer >= stuckThreshold) {
        state = PathfindingState::Stuck;
    }
}

float PathfindingComponent::GetPathProgress() const
{
    if (waypoints.empty()) return 1.0f;
    return static_cast<float>(currentWaypointIndex) / static_cast<float>(waypoints.size());
}

int PathfindingComponent::GetRemainingWaypointCount() const
{
    return std::max(0, static_cast<int>(waypoints.size()) - currentWaypointIndex);
}

PathfindingState PathfindingComponent::GetState() const
{
    return state;
}

} // namespace components
} // namespace engine
