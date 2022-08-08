#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <cstdint>
#include "../math/Vector2.h"
#include "../math/Rect.h"
#include "PhysicsWorld.h"

namespace engine { namespace physics {

struct GridCell {
    int cx, cy;
    bool operator==(const GridCell& o) const { return cx == o.cx && cy == o.cy; }
};

struct GridCellHash {
    std::size_t operator()(const GridCell& c) const {
        return std::hash<int>()(c.cx) ^ (std::hash<int>()(c.cy) << 1);
    }
};

class BroadPhase {
public:
    explicit BroadPhase(float cellSize = 128.0f);

    void rebuild(const std::vector<std::unique_ptr<RigidBody>>& bodies);
    std::vector<std::pair<RigidBody*, RigidBody*>> getPotentialPairs() const;
    std::vector<RigidBody*> queryRegion(const engine::math::Rectf& region) const;

private:
    float mCellSize;
    std::unordered_map<GridCell, std::vector<RigidBody*>, GridCellHash> mGrid;

    GridCell getCell(const engine::math::Vector2f& pos) const;
    void getSurroundingCells(const GridCell& center, std::vector<GridCell>& out) const;
};

}} // namespace engine::physics
