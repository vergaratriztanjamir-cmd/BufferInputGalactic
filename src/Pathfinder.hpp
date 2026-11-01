
#pragma once

#include "Physics.hpp"

#include <vector>
#include <algorithm>
#include <cstddef>

namespace Galactic {

struct PathPoint {
    Vec2 position;
    int frame;
};

class Pathfinder {
public:

    static std::vector<PathPoint> predict(
        PhysicsState state,
        int frames,
        float deltaTime,
        size_t budget = 256
    ) {
        std::vector<PathPoint> path;

        frames = std::max(0, frames);

        size_t count = std::min(
            static_cast<size_t>(frames) + 1,
            budget
        );

        path.reserve(count);

        if (count == 0)
            return path;

        path.push_back({
            state.position,
            0
        });

        for (size_t i = 1; i < count; i++) {
            state = simulateStep(
                state,
                deltaTime
            );

            path.push_back({
                state.position,
                static_cast<int>(i)
            });
        }

        return path;
    }

    static bool hasCollision(
        Vec2 player,
        float playerRadius,
        Vec2 obstacle,
        float obstacleRadius
    ) {
        return circlesOverlap(
            player,
            playerRadius,
            obstacle,
            obstacleRadius
        );
    }
};

}
