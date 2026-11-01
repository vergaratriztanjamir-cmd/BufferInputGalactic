
#pragma once

#include <cmath>
#include <algorithm>

namespace Galactic {

struct Vec2 {
    float x = 0.f;
    float y = 0.f;
};

struct PhysicsState {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
};

inline PhysicsState simulateStep(
    PhysicsState state,
    float deltaTime
) {
    state.velocity.x +=
        state.acceleration.x * deltaTime;

    state.velocity.y +=
        state.acceleration.y * deltaTime;

    state.position.x +=
        state.velocity.x * deltaTime;

    state.position.y +=
        state.velocity.y * deltaTime;

    return state;
}

inline float distanceSquared(
    Vec2 a,
    Vec2 b
) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return dx * dx + dy * dy;
}

inline bool circlesOverlap(
    Vec2 a,
    float radiusA,
    Vec2 b,
    float radiusB
) {
    float radius =
        std::max(0.f, radiusA) +
        std::max(0.f, radiusB);

    return distanceSquared(a, b)
        <= radius * radius;
}

}
