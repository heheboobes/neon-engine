#include "BroadPhase.h"
#include <set>
#include <algorithm>

namespace engine { namespace physics {

BroadPhase::BroadPhase(float cellSize)
    : mCellSize(cellSize)
{
}

GridCell BroadPhase::getCell(const engine::math::Vector2f& pos) const {
    GridCell cell;
    cell.cx = static_cast<int>(std::floor(pos.x / mCellSize));
    cell.cy = static_cast<int>(std::floor(pos.y / mCellSize));
    return cell;
}

void BroadPhase::getSurroundingCells(const GridCell& center, std::vector<GridCell>& out) const {
    out.clear();
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            out.push_back({center.cx + dx, center.cy + dy});
        }
    }
}

void BroadPhase::rebuild(const std::vector<std::unique_ptr<RigidBody>>& bodies) {
    mGrid.clear();

    for (const auto& body : bodies) {
        auto aabb = body->getAABB();
        GridCell minCell = getCell(engine::math::Vector2f(aabb.left(), aabb.top()));
        GridCell maxCell = getCell(engine::math::Vector2f(aabb.right(), aabb.bottom()));

        for (int cx = minCell.cx; cx <= maxCell.cx; ++cx) {
            for (int cy = minCell.cy; cy <= maxCell.cy; ++cy) {
                GridCell cell{cx, cy};
                mGrid[cell].push_back(body.get());
            }
        }
    }
}

std::vector<std::pair<RigidBody*, RigidBody*>> BroadPhase::getPotentialPairs() const {
    std::set<std::pair<RigidBody*, RigidBody*>> uniquePairs;

    for (const auto& [cell, bodies] : mGrid) {
        std::vector<GridCell> neighbors;
        getSurroundingCells(cell, neighbors);

        for (const auto& neighbor : neighbors) {
            auto it = mGrid.find(neighbor);
            if (it == mGrid.end()) continue;

            for (auto* a : bodies) {
                for (auto* b : it->second) {
                    if (a == b) continue;
                    auto pair = std::minmax(a, b);
                    uniquePairs.insert(pair);
                }
            }
        }
    }

    return std::vector<std::pair<RigidBody*, RigidBody*>>(
        uniquePairs.begin(), uniquePairs.end()
    );
}

std::vector<RigidBody*> BroadPhase::queryRegion(const engine::math::Rectf& region) const {
    std::set<RigidBody*> found;

    GridCell minCell = getCell(engine::math::Vector2f(region.left(), region.top()));
    GridCell maxCell = getCell(engine::math::Vector2f(region.right(), region.bottom()));

    for (int cx = minCell.cx; cx <= maxCell.cx; ++cx) {
        for (int cy = minCell.cy; cy <= maxCell.cy; ++cy) {
            auto it = mGrid.find({cx, cy});
            if (it == mGrid.end()) continue;
            for (auto* body : it->second) {
                if (body->getAABB().intersects(region)) {
                    found.insert(body);
                }
            }
        }
    }

    return std::vector<RigidBody*>(found.begin(), found.end());
}

}} // namespace engine::physics
