
#include <Geode/Geode.hpp>

#include "ModeSettings.hpp"
#include "Physics.hpp"
#include "InputBuffer.hpp"
#include "Pathfinder.hpp"

using namespace geode::prelude;

namespace Galactic {

    ModeSettings g_settings;
    InputBuffer g_inputBuffer;

    bool g_initialized = false;

    void initializeGalactic() {

        if (g_initialized)
            return;

        g_settings.loadFromGeode();

        g_initialized = true;

        log::info("========== GALACTIC ==========");

        log::info("Android64 build");

        log::info("8 game modes initialized");

        log::info("Dual-player profiles loaded");

        log::info("Input buffer core initialized");

        log::info("Pathfinder core initialized");

        log::info("Galactic startup complete!");

        log::info("==============================");
    }
}

$on_mod(Loaded) {
    Galactic::initializeGalactic();
}
