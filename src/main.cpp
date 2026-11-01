
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <chrono>

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

    bool g_initialized = false;
    bool g_firstInputLogged = false;
    bool g_firstProcessedLogged = false;

    int g_lastMode[2] = {-1, -1};

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
        log::info("8 game modes loaded.");
        log::info("Dual-player profiles loaded.");
        log::info("Input recording initialized.");
        log::info("Game-mode detection initialized.");
        log::info("Input queue processing initialized.");
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

        if (!buttonHook) {
            log::error("GALACTIC: Input hook registration FAILED!");
        }
        else {
            log::info("GALACTIC: Input hook registered!");
        }

        auto updateHook = self.getHook(
            "GJBaseGameLayer::update"
        );

        if (!updateHook) {
            log::error("GALACTIC: Update hook registration FAILED!");
        }
        else {
            log::info("GALACTIC: Update hook registered!");
        }
    }

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        uint64_t now = Galactic::getTimestampMs();

        // Remove inputs that have exceeded their timing window.
        Galactic::g_inputBuffer.update(now);

        // Evaluate every input that remains in the queue.
        Galactic::ProcessedInput result;

        while (Galactic::g_inputProcessor.processNext(
            Galactic::g_inputBuffer,
            now,
            result
        )) {
            if (!Galactic::g_firstProcessedLogged) {
                Galactic::g_firstProcessedLogged = true;

                log::info(
                    "GALACTIC: Input processor active! Player={}, mode={}, withinWindow={}",
                    result.input.player,
                    result.input.mode,
                    result.withinWindow
                );
            }
        }
    }

    void handleButton(bool down, int button, bool isPlayer1) {

        if (down) {

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
                    profile.bufferWindowMs
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
            else {
                log::warn(
                    "GALACTIC: Player {} pointer is null!",
                    playerNumber
                );
            }
        }

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};
