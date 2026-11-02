#pragma once
#include <vector>
#include <algorithm>
#include "Physics.hpp"
#include "ModeSettings.hpp"

namespace Galactic {
struct PathPoint { Vec2 position; int frame = 0; };

struct PredictionProfile {
    int player = 1;
    GameMode mode = GameMode::Cube;
    int ticks = 1;
    int frameWindow = 2;
    int bufferWindowMs = 50;
};

namespace Pathfinder {
inline PredictionProfile makeProfile(int player, GameMode mode,
                                     const ModeSettings& settings) {
    const auto p = settings.get(player, mode);
    return {player, mode, p.ticks, p.frameWindow, p.bufferWindowMs};
}

inline std::vector<PathPoint> predict(const PhysicsState& initial, int frames,
                                      float deltaTime, std::size_t budget = 256) {
    std::vector<PathPoint> path;
    if (frames < 0 || deltaTime <= 0.f || budget == 0) return path;
    const int count = std::min<int>(frames, static_cast<int>(budget - 1));
    path.reserve(static_cast<std::size_t>(count + 1));
    auto state = initial;
    path.push_back({state.position, 0});
    for (int frame = 1; frame <= count; ++frame) {
        state = simulateStep(state, deltaTime);
        path.push_back({state.position, frame});
    }
    return path;
}

inline std::vector<PathPoint> predictForMode(const PhysicsState& state,
                                              const PredictionProfile& profile,
                                              float deltaTime,
                                              std::size_t budget = 256) {
    const int frames = std::clamp(profile.frameWindow, 0, 240);
    return predict(state, frames, deltaTime, budget);
}

inline bool hasCollision(Vec2 a, float radiusA, Vec2 b, float radiusB) {
    return circlesOverlap(a, radiusA, b, radiusB);
}
}
}
