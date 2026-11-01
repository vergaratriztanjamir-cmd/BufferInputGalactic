
#pragma once

#include <Geode/Geode.hpp>

#include <array>
#include <cstdint>
#include <algorithm>
#include <string>

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
    std::array<ModeProfile, MODE_COUNT> modes{};
};

class ModeSettings {
public:

    PlayerProfiles player1;
    PlayerProfiles player2;

    ModeProfile& get(int player, GameMode mode) {
        size_t index = static_cast<size_t>(mode);

        index = std::min(index, MODE_COUNT - 1);

        return player == 2
            ? player2.modes[index]
            : player1.modes[index];
    }

    const ModeProfile& get(
        int player,
        GameMode mode
    ) const {
        size_t index = static_cast<size_t>(mode);

        index = std::min(index, MODE_COUNT - 1);

        return player == 2
            ? player2.modes[index]
            : player1.modes[index];
    }

    void loadFromGeode() {

        static constexpr const char* names[] = {
            "cube",
            "ship",
            "ball",
            "ufo",
            "wave",
            "robot",
            "spider",
            "swing"
        };

        auto mod = geode::Mod::get();

        for (int player = 1; player <= 2; player++) {

            for (size_t i = 0; i < MODE_COUNT; i++) {

                auto& profile = get(
                    player,
                    static_cast<GameMode>(i)
                );

                std::string prefix =
                    "p" + std::to_string(player) +
                    "-" + names[i] + "-";

                auto ticks = mod->getSettingValue<int64_t>(
                    prefix + "ticks"
                );

                auto frames = mod->getSettingValue<int64_t>(
                    prefix + "frames"
                );

                auto window = mod->getSettingValue<int64_t>(
                    prefix + "window"
                );

                profile.ticks = static_cast<int>(
                    std::clamp<int64_t>(ticks, 0, 12)
                );

                profile.frameWindow = static_cast<int>(
                    std::clamp<int64_t>(frames, 0, 30)
                );

                profile.bufferWindowMs = static_cast<int>(
                    std::clamp<int64_t>(window, 0, 250)
                );
            }
        }

        geode::log::info(
            "Galactic: loaded 48 timing settings."
        );
    }
};

}
