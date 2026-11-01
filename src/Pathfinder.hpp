
#pragma once

#include "Physics.hpp"
#include "ModeSettings.hpp"

#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace Galactic {

struct PathPoint {
    Vec2 position;
    int frame;
};

struct PredictionProfile {
    int player = 1;
    GameMode mode = GameMode::Cube;
    int ticks = 1;
    int frameWindow = 2;
    int bufferWindowMs = 50;
};

class Pathfinder {
public:

    static PredictionProfile makeProfile(
        int player,
        GameMode mode,
        const ModeSettings& settings
    ) {
        const auto& timing = settings.get(player, mode);

        PredictionProfile profile;

        profile.player = player;
        profile.mode = mode;
        profile.ticks = std::clamp(timing.ticks, 0, 12);
        profile.frameWindow = std::clamp(
            timing.frameWindow, 0, 30
        );
        profile.bufferWindowMs = std::clamp(
            timing.bufferWindowMs, 0, 250
        );

        return profile;
    }

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
            state = simulateStep(state, deltaTime);

            path.push_back({
                state.position,
                static_cast<int>(i)
            });
        }

        return path;
    }

    static std::vector<PathPoint> predictForMode(
        PhysicsState state,
        const PredictionProfile& profile,
        float deltaTime,
        size_t budget = 256
    ) {
        int frames = std::clamp(
            profile.frameWindow,
            0,
            30
        );

        return predict(
            state,
            frames,
            deltaTime,
            budget
        );
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
