
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
    bool velocityValid = false;
};

class PhysicsTracker {
private:
    PlayerSnapshot previous[2];
    PlayerSnapshot current[2];

public:
    bool capture(
        int playerIndex,
        cocos2d::CCPoint position,
        float deltaTime
    ) {
        return captureInternal(
            playerIndex,
            position,
            deltaTime,
            0.0f,
            0.0f,
            false
        );
    }

    bool captureWithActualVelocity(
        int playerIndex,
        cocos2d::CCPoint position,
        float deltaTime,
        double actualXVelocity,
        double actualYVelocity
    ) {
        return captureInternal(
            playerIndex,
            position,
            deltaTime,
            static_cast<float>(actualXVelocity),
            static_cast<float>(actualYVelocity),
            true
        );
    }

private:
    bool captureInternal(
        int playerIndex,
        cocos2d::CCPoint position,
        float deltaTime,
        float actualXVelocity,
        float actualYVelocity,
        bool hasActualVelocity
    ) {
        if (playerIndex < 0 || playerIndex > 1) {
            return false;
        }

        PlayerSnapshot next;
        next.x = position.x;
        next.y = position.y;
        next.valid = true;

        if (
            current[playerIndex].valid &&
            deltaTime > 0.0001f
        ) {
            previous[playerIndex] = current[playerIndex];

            if (hasActualVelocity) {
                next.velocityX = actualXVelocity;
                next.velocityY = actualYVelocity;
            } else {
                next.velocityX =
                    (next.x - previous[playerIndex].x) / deltaTime;

                next.velocityY =
                    (next.y - previous[playerIndex].y) / deltaTime;
            }

            next.velocityValid = true;
        }

        current[playerIndex] = next;
        return next.velocityValid;
    }

public:
    const PlayerSnapshot& get(int playerIndex) const {
        static const PlayerSnapshot empty{};

        if (playerIndex < 0 || playerIndex > 1) {
            return empty;
        }

        return current[playerIndex];
    }

    const PlayerSnapshot& getPrevious(int playerIndex) const {
        static const PlayerSnapshot empty{};

        if (playerIndex < 0 || playerIndex > 1) {
            return empty;
        }

        return previous[playerIndex];
    }

    void reset() {
        for (int i = 0; i < 2; i++) {
            previous[i] = PlayerSnapshot{};
            current[i] = PlayerSnapshot{};
        }
    }
};

}
