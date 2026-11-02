#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <chrono>
#include "InputBuffer.hpp"
#include "InputProcessor.hpp"
#include "ModeSettings.hpp"
#include "Physics.hpp"
#include "Pathfinder.hpp"

using namespace geode::prelude;

namespace Galactic {
static InputBuffer g_inputBuffer;
static InputProcessor g_inputProcessor;
static ModeSettings g_modeSettings;
static PhysicsTracker g_physics;

static uint64_t nowMs() {
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count()
    );
}

// This starter hook confirms the mod loads. Gameplay input and player physics
// hooks must be connected to verified Geode/GD APIs before claiming prediction
// or collision warnings are active.
}

class $modify(GalacticPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        log::info("Galactic initialized. Input and prediction modules ready.");
        return true;
    }

    void onQuit() {
        Galactic::g_inputBuffer.clear();
        Galactic::g_inputProcessor.reset();
        Galactic::g_physics.reset();
        PlayLayer::onQuit();
    }
};
