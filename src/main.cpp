#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <chrono>
#include <algorithm>
#include <cstdint>

#include "ModeSettings.hpp"
#include "Physics.hpp"
#include "InputBuffer.hpp"
#include "InputProcessor.hpp"
#include "Pathfinder.hpp"

using namespace geode::prelude;

namespace Galactic {

ModeSettings g_settings;
InputBuffer g_inputBuffer;
InputProcessor g_inputProcessor;
PhysicsTracker g_physicsTracker;

bool g_initialized = false;
bool g_firstInputLogged = false;

bool g_firstDownProcessedLogged = false;
bool g_firstUpProcessedLogged = false;

bool g_firstVelocityLogged[2] = {false, false};
bool g_firstPathLogged[2] = {false, false};

int g_lastMode[2] = {-1, -1};
int g_lastPredictionMode[2] = {-1, -1};
int g_lastPredictionFrames[2] = {-1, -1};

uint64_t getTimestampMs() {
    auto now = std::chrono::steady_clock::now();

    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count()
    );
}

GameMode detectMode(PlayerObject* player) {
    if (!player) return GameMode::Cube;
    if (player->m_isShip) return GameMode::Ship;
    if (player->m_isBall) return GameMode::Ball;
    if (player->m_isBird) return GameMode::UFO;
    if (player->m_isDart) return GameMode::Wave;
    if (player->m_isRobot) return GameMode::Robot;
    if (player->m_isSpider) return GameMode::Spider;
    if (player->m_isSwing) return GameMode::Swing;

    return GameMode::Cube;
}

const char* modeName(GameMode mode) {
    switch (mode) {
        case GameMode::Cube: return "Cube";
        case GameMode::Ship: return "Ship";
        case GameMode::Ball: return "Ball";
        case GameMode::UFO: return "UFO";
        case GameMode::Wave: return "Wave";
        case GameMode::Robot: return "Robot";
        case GameMode::Spider: return "Spider";
        case GameMode::Swing: return "Swing";
        default: return "Unknown";
    }
}

void initializeGalactic() {
    if (g_initialized) return;

    g_settings.loadFromGeode();
    g_initialized = true;

    log::info("Galactic initialized!");
    log::info("Physics Tracking v0.5 initialized.");
    log::info("Actual horizontal and vertical velocity sampling enabled.");
    log::info("Mode-aware Pathfinder initialized.");
    log::info("Press and release input recording enabled.");
    log::info("Separate press/release processing logs enabled.");
}

void predictPlayer(int playerIndex, PlayerObject* playerObject) {
    if (playerIndex < 0 || playerIndex > 1 || !playerObject) {
        return;
    }

    const auto& snapshot = g_physicsTracker.get(playerIndex);

    if (!snapshot.valid || !snapshot.velocityValid) {
        return;
    }

    int playerNumber = playerIndex + 1;
    GameMode mode = detectMode(playerObject);

    PredictionProfile profile = Pathfinder::makeProfile(
        playerNumber,
        mode,
        g_settings
    );

    PhysicsState state;

    state.position = {snapshot.x, snapshot.y};
    state.velocity = {snapshot.velocityX, snapshot.velocityY};
    state.gravity = 0.0f;

    auto mod = geode::Mod::get();

    int64_t horizon = mod->getSettingValue<int64_t>(
        "prediction-horizon"
    );

    int64_t budget = mod->getSettingValue<int64_t>(
        "pathfinder-budget"
    );

    horizon = std::clamp<int64_t>(horizon, 1, 240);
    budget = std::clamp<int64_t>(budget, 32, 2048);

    int predictionFrames = std::min(
        profile.frameWindow,
        static_cast<int>(horizon)
    );

    constexpr float FRAME_TIME = 1.0f / 240.0f;

    auto path = Pathfinder::predictForMode(
        state,
        profile,
        FRAME_TIME,
        static_cast<size_t>(budget)
    );

    if (path.size() >
        static_cast<size_t>(predictionFrames) + 1) {
        path.resize(
            static_cast<size_t>(predictionFrames) + 1
        );
    }

    if (!g_firstPathLogged[playerIndex] && !path.empty()) {
        g_firstPathLogged[playerIndex] = true;

        log::info(
            "GALACTIC: P{} mode-aware path generated! Mode={}, Points={}, Frames={}",
            playerNumber,
            modeName(mode),
            path.size(),
            predictionFrames
        );
    }

    if (
        g_lastPredictionMode[playerIndex] != static_cast<int>(mode) ||
        g_lastPredictionFrames[playerIndex] != predictionFrames
    ) {
        g_lastPredictionMode[playerIndex] = static_cast<int>(mode);
        g_lastPredictionFrames[playerIndex] = predictionFrames;

        log::info(
            "GALACTIC: P{} prediction profile: Mode={}, Frames={}, Ticks={}, Window={}ms",
            playerNumber,
            modeName(mode),
            predictionFrames,
            profile.ticks,
            profile.bufferWindowMs
        );
    }
}

}

$on_mod(Loaded) {
Galactic::initializeGalactic();
}

class $modify(GalacticGameLayer, GJBaseGameLayer) {

static void onModify(auto& self) {
    auto buttonHook = self.getHook(
        "GJBaseGameLayer::handleButton"
    );

    if (!buttonHook)
        log::error("GALACTIC: Input hook registration FAILED!");
    else
        log::info("GALACTIC: Input hook registered!");

    auto updateHook = self.getHook(
        "GJBaseGameLayer::update"
    );

    if (!updateHook)
        log::error("GALACTIC: Update hook registration FAILED!");
    else
        log::info("GALACTIC: Update hook registered!");
}

void update(float dt) {
    GJBaseGameLayer::update(dt);

    uint64_t now = Galactic::getTimestampMs();

    if (m_player1) {
        double actualXVelocity =
            m_player1->getCurrentXVelocity() *
            m_player1->reverseMod();

        Galactic::g_physicsTracker.captureWithActualVelocity(
            0,
            m_player1->getPosition(),
            dt,
            actualXVelocity,
            m_player1->m_yVelocity
        );

        const auto& snapshot = Galactic::g_physicsTracker.get(0);

        if (
            snapshot.velocityValid &&
            !Galactic::g_firstVelocityLogged[0]
        ) {
            Galactic::g_firstVelocityLogged[0] = true;

            log::info(
                "GALACTIC: P1 actual velocity! vx={}, vy={}",
                snapshot.velocityX,
                snapshot.velocityY
            );
        }

        Galactic::predictPlayer(0, m_player1);
    }

    if (m_player2 && m_gameState.m_isDualMode) {
        double actualXVelocity =
            m_player2->getCurrentXVelocity() *
            m_player2->reverseMod();

        Galactic::g_physicsTracker.captureWithActualVelocity(
            1,
            m_player2->getPosition(),
            dt,
            actualXVelocity,
            m_player2->m_yVelocity
        );

        const auto& snapshot = Galactic::g_physicsTracker.get(1);

        if (
            snapshot.velocityValid &&
            !Galactic::g_firstVelocityLogged[1]
        ) {
            Galactic::g_firstVelocityLogged[1] = true;

            log::info(
                "GALACTIC: P2 actual velocity! vx={}, vy={}",
                snapshot.velocityX,
                snapshot.velocityY
            );
        }

        Galactic::predictPlayer(1, m_player2);
    }

    Galactic::g_inputBuffer.update(now);

    Galactic::ProcessedInput result;

    while (Galactic::g_inputProcessor.processNext(
        Galactic::g_inputBuffer,
        now,
        result
    )) {

        if (
            result.input.down &&
            !Galactic::g_firstDownProcessedLogged
        ) {
            Galactic::g_firstDownProcessedLogged = true;

            log::info(
                "GALACTIC: BUTTON PRESS PROCESSED! Player={}, mode={}, withinWindow={}",
                result.input.player,
                result.input.mode,
                result.withinWindow
            );
        }

        if (
            !result.input.down &&
            !Galactic::g_firstUpProcessedLogged
        ) {
            Galactic::g_firstUpProcessedLogged = true;

            log::info(
                "GALACTIC: BUTTON RELEASE PROCESSED! Player={}, mode={}, withinWindow={}",
                result.input.player,
                result.input.mode,
                result.withinWindow
            );
        }
    }
}

void handleButton(bool down, int button, bool isPlayer1) {
    bool isDual = m_gameState.m_isDualMode;

    int playerNumber = isDual
        ? (isPlayer1 ? 1 : 2)
        : 1;

    PlayerObject* playerObject = playerNumber == 1
        ? m_player1
        : m_player2;

    if (playerObject) {
        auto mode = Galactic::detectMode(playerObject);
        int modeIndex = static_cast<int>(mode);

        const auto& profile =
            Galactic::g_settings.get(playerNumber, mode);

        Galactic::g_inputBuffer.push(
            Galactic::getTimestampMs(),
            playerNumber,
            modeIndex,
            button,
            profile.ticks,
            profile.frameWindow,
            profile.bufferWindowMs,
            down
        );

        int slot = playerNumber - 1;

        if (Galactic::g_lastMode[slot] != modeIndex) {
            Galactic::g_lastMode[slot] = modeIndex;

            log::info(
                "GALACTIC: Player {} mode detected: {}",
                playerNumber,
                Galactic::modeName(mode)
            );

            log::info(
                "GALACTIC: P{} timing: ticks={}, frames={}, window={}ms",
                playerNumber,
                profile.ticks,
                profile.frameWindow,
                profile.bufferWindowMs
            );
        }

        if (!Galactic::g_firstInputLogged) {
            Galactic::g_firstInputLogged = true;
            log::info("GALACTIC: Input recorded!");
        }
    }

    GJBaseGameLayer::handleButton(down, button, isPlayer1);
}

};