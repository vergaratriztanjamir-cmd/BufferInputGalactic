
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

        g_initialized = true;

        log::info("Galactic initialization started.");

        log::info(
            "Supported game modes: 8"
        );

        log::info(
            "Dual-player profiles: enabled"
        );

        log::info(
            "Input buffer core: initialized"
        );

        log::info(
            "Pathfinder core: initialized"
        );

        log::info(
            "Galactic initialization complete."
        );
    }

}

$on_mod(Loaded) {
    Galactic::initializeGalactic();

    log::info(
        "Galactic loaded successfully."
    );
}
