
#pragma once

#include <array>
#include <cstdint>
#include <algorithm>

namespace Galactic {

enum class GameMode : uint8_t {
    Cube,
    Ship,
    Ball,
    UFO,
    Wave,
    Robot,
    Spider,
    Swing,
    Count
};

constexpr size_t MODE_COUNT =
    static_cast<size_t>(GameMode::Count);

struct ModeProfile {
    int ticks = 1;
    int frameWindow = 2;
    int bufferWindowMs = 50;
};

struct PlayerProfiles {
    std::array<ModeProfile, MODE_COUNT> modes;
};

class ModeSettings {
public:
    PlayerProfiles player1;
    PlayerProfiles player2;

    ModeProfile& get(
        int player,
        GameMode mode
    ) {
        auto index = static_cast<size_t>(mode);

        index = std::min(
            index,
            MODE_COUNT - 1
        );

        return player == 2
            ? player2.modes[index]
            : player1.modes[index];
    }

    const ModeProfile& get(
        int player,
        GameMode mode
    ) const {
        auto index = static_cast<size_t>(mode);

        index = std::min(
            index,
            MODE_COUNT - 1
        );

        return player == 2
            ? player2.modes[index]
            : player1.modes[index];
    }
};

}
