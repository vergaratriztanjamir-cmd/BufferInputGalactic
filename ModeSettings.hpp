#pragma once
#include <array>
#include <algorithm>
#include <cstdint>

namespace Galactic {
enum class GameMode : int { Cube, Ship, Ball, UFO, Wave, Robot, Spider, Swing, Count };

struct ModeProfile {
    int ticks = 1;
    int frameWindow = 2;
    int bufferWindowMs = 50;
};

struct PlayerProfiles {
    std::array<ModeProfile, static_cast<std::size_t>(GameMode::Count)> modes{};
};

class ModeSettings {
private:
    PlayerProfiles players[2]{};

public:
    ModeProfile get(int player, GameMode mode) const {
        if (player < 1 || player > 2) return {};
        auto index = static_cast<std::size_t>(mode);
        if (index >= static_cast<std::size_t>(GameMode::Count)) return {};
        return players[player - 1].modes[index];
    }

    void set(int player, GameMode mode, ModeProfile profile) {
        if (player < 1 || player > 2) return;
        auto index = static_cast<std::size_t>(mode);
        if (index >= static_cast<std::size_t>(GameMode::Count)) return;
        profile.ticks = std::clamp(profile.ticks, 0, 12);
        profile.frameWindow = std::clamp(profile.frameWindow, 0, 240);
        profile.bufferWindowMs = std::clamp(profile.bufferWindowMs, 0, 250);
        players[player - 1].modes[index] = profile;
    }

    void resetDefaults() {
        for (auto& player : players)
            for (auto& mode : player.modes) mode = ModeProfile{};
    }
};
}
