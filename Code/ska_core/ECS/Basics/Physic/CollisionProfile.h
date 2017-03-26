#pragma once
#include <vector>
#include <unordered_set>
#include "Rectangle.h"
#include "Point.h"

namespace ska {
    class CollisionProfile {
    public:
        CollisionProfile() = default;
        virtual ~CollisionProfile() = default;

        virtual bool canMoveToPos(const Rectangle& hitbox, std::vector<ska::Point<int>>& output) const = 0;
        virtual bool canMoveOnBlock(const Point<int>& pos, const std::unordered_set<int>& blocksAuthorized, int i) const = 0;
    };
}
