
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

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

    void initializeGalactic() {
        if (g_initialized) return;

        g_settings.loadFromGeode();
        g_initialized = true;

        log::info("Galactic initialized!");
        log::info("8 game modes loaded.");
        log::info("Dual-player profiles loaded.");
        log::info("Input buffer core initialized.");
        log::info("Pathfinder core initialized.");
    }
}

$on_mod(Loaded) {
    Galactic::initializeGalactic();
}

class $modify(GalacticGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        if (down && !Galactic::g_firstInputLogged) {
            Galactic::g_firstInputLogged = true;
            log::info("GALACTIC: GAMEPLAY INPUT DETECTED!");
        }

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};
