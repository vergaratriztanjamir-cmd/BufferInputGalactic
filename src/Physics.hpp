#pragma once
#include <cmath>
#include "Geode/Geode.hpp"

namespace Galactic {
struct Vec2 { float x = 0.f, y = 0.f; };
struct PhysicsState { Vec2 position; Vec2 velocity; float gravity = 0.f; };

inline PhysicsState simulateStep(PhysicsState state, float deltaTime) {
    state.position.x += state.velocity.x * deltaTime;
    state.position.y += state.velocity.y * deltaTime;
    state.velocity.y += state.gravity * deltaTime;
    return state;
}

inline bool circlesOverlap(Vec2 a, float radiusA, Vec2 b, float radiusB) {
    const float dx = a.x - b.x, dy = a.y - b.y;
    const float r = radiusA + radiusB;
    return dx * dx + dy * dy <= r * r;
}

struct PlayerSnapshot {
    float x = 0.f, y = 0.f, velocityX = 0.f, velocityY = 0.f;
    bool valid = false, velocityValid = false;
};

class PhysicsTracker {
private:
    PlayerSnapshot previous[2]{}, current[2]{};

public:
    bool captureWithActualVelocity(int player, cocos2d::CCPoint pos, float,
                                   double vx, double vy) {
        if (player < 1 || player > 2) return false;
        const int i = player - 1;
        previous[i] = current[i];
        current[i] = {pos.x, pos.y, static_cast<float>(vx),
                      static_cast<float>(vy), true, true};
        return true;
    }

    const PlayerSnapshot& get(int player) const {
        static const PlayerSnapshot empty{};
        return player >= 1 && player <= 2 ? current[player - 1] : empty;
    }

    const PlayerSnapshot& getPrevious(int player) const {
        static const PlayerSnapshot empty{};
        return player >= 1 && player <= 2 ? previous[player - 1] : empty;
    }

    void reset() {
        for (int i = 0; i < 2; ++i) previous[i] = current[i] = {};
    }
};
}
