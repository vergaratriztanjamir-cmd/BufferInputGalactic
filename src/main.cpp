
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <chrono>

#include "ModeSettings.hpp"
#include "Physics.hpp"
#include "InputBuffer.hpp"
#include "Pathfinder.hpp"

using namespace geode::prelude;

namespace Galactic {
    ModeSettings g_settings;
    InputBuffer g_inputBuffer;

    bool g_initialized = false;
    bool g_firstInputLogged = false;

    uint64_t getTimestampMs() {
        auto now = std::chrono::steady_clock::now();

        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()
            ).count()
        );
    }

    void initializeGalactic() {
        if (g_initialized) return;

        g_settings.loadFromGeode();
        g_initialized = true;

        log::info("Galactic initialized!");
        log::info("8 game modes loaded.");
        log::info("Dual-player profiles loaded.");
        log::info("Input recording initialized.");
    }
}

$on_mod(Loaded) {
    Galactic::initializeGalactic();
}

class $modify(GalacticGameLayer, GJBaseGameLayer) {

    static void onModify(auto& self) {
        auto hook = self.getHook("GJBaseGameLayer::handleButton");

        if (!hook) {
            log::error("GALACTIC: Hook registration FAILED!");
            return;
        }

        log::info("GALACTIC: Input hook registered!");
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        if (down) {
            int player = isPlayer1 ? 1 : 2;

            Galactic::g_inputBuffer.push(
                Galactic::getTimestampMs(),
                player,
                button
            );

            if (!Galactic::g_firstInputLogged) {
                Galactic::g_firstInputLogged = true;
                log::info("GALACTIC: Input recorded!");
            }
        }

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};
