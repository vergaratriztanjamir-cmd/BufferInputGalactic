
#pragma once

#include <Geode/Geode.hpp>
#include <cmath>

namespace Galactic {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct PhysicsState {
    Vec2 position;
    Vec2 velocity;
    float gravity = 0.0f;
};

inline PhysicsState simulateStep(
    PhysicsState state,
    float deltaTime
) {
    state.position.x += state.velocity.x * deltaTime;
    state.position.y += state.velocity.y * deltaTime;

    state.velocity.y += state.gravity * deltaTime;

    return state;
}

inline bool circlesOverlap(
    Vec2 a,
    float radiusA,
    Vec2 b,
    float radiusB
) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    float radiusSum = radiusA + radiusB;

    return (dx * dx + dy * dy) <=
        (radiusSum * radiusSum);
}

struct PlayerSnapshot {
    float x = 0.0f;
    float y = 0.0f;

    float velocityX = 0.0f;
    float velocityY = 0.0f;

    bool valid = false;
};

class PhysicsTracker {
private:
    PlayerSnapshot previous[2];
    PlayerSnapshot current[2];

public:
    void capture(int playerIndex, cocos2d::CCPoint position) {
        if (playerIndex < 0 || playerIndex > 1) return;

        PlayerSnapshot next;
        next.x = position.x;
        next.y = position.y;
        next.valid = true;

        if (current[playerIndex].valid) {
            previous[playerIndex] = current[playerIndex];

            next.velocityX =
                next.x - previous[playerIndex].x;

            next.velocityY =
                next.y - previous[playerIndex].y;
        }

        current[playerIndex] = next;
    }

    const PlayerSnapshot& get(int playerIndex) const {
        static const PlayerSnapshot empty{};

        if (playerIndex < 0 || playerIndex > 1) {
            return empty;
        }

        return current[playerIndex];
    }

    void reset() {
        for (int i = 0; i < 2; i++) {
            previous[i] = PlayerSnapshot{};
            current[i] = PlayerSnapshot{};
        }
    }
};

}
